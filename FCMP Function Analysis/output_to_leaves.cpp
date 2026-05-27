void CurveTrees<C1, C2>::outputs_to_leaves(std::vector<UnifiedOutput>&& new_outputs,
    std::vector<typename C1::Scalar>& flattened_leaves_out,
    std::vector<UnifiedOutput>& valid_outputs_out)
{
    flattened_leaves_out.clear();
    valid_outputs_out.clear();

    TIME_MEASURE_START(set_valid_leaves);

    // Keep track of valid outputs to make sure we only use leaves from valid outputs. Can't use std::vector<bool>
    // because std::vector<bool> concurrent access is not thread safe.
    enum Boolean : uint8_t {
        False = 0,
        True = 1,
    };
    std::vector<Boolean> valid_outputs(new_outputs.size(), False);

    tools::threadpool& tpool = tools::threadpool::getInstanceForCompute();
    tools::threadpool::waiter waiter(tpool);
    const std::size_t n_threads = std::max<std::size_t>(1, tpool.get_max_concurrency());

    TIME_MEASURE_START(convert_valid_leaves);
    // Step 1. Multithreaded convert valid outputs into Edwards derivatives needed to get Wei coordinates
    std::vector<PreLeafTuple> pre_leaves;
    pre_leaves.resize(new_outputs.size());
    const std::size_t LEAF_CONVERT_BATCH_SIZE = std::max<std::size_t>(1, (new_outputs.size() / n_threads));
    for (std::size_t i = 0; i < new_outputs.size(); i += LEAF_CONVERT_BATCH_SIZE)
    {
        const std::size_t end = std::min(i + LEAF_CONVERT_BATCH_SIZE, new_outputs.size());
        tpool.submit(&waiter,
            [
                &new_outputs,
                &valid_outputs,
                &pre_leaves,
                i,
                end
            ]()
            {
                for (std::size_t j = i; j < end; ++j)
                {
                    CHECK_AND_ASSERT_THROW_MES(!valid_outputs.at(j), "unexpected already valid output");

                    const auto& output_pair = new_outputs.at(j).output_pair;
                    try
                    {
                        pre_leaves.at(j) = output_to_pre_leaf_tuple(output_pair);
                    }
                    catch (...)
                    {
                        /* Invalid outputs can't be added to the tree */
                        LOG_PRINT_L2("Output " << new_outputs.at(j).unified_id << " is invalid (out pubkey "
                            << output_pubkey_cref(output_pair)
                            << " , commitment " << commitment_cref(output_pair) << ")");
                        continue;
                    }

                    valid_outputs.at(j) = True;
                }
            },
            true
        );
    }

    CHECK_AND_ASSERT_THROW_MES(waiter.wait(), "failed to convert outputs to ed derivatives");
    TIME_MEASURE_FINISH(convert_valid_leaves);

    TIME_MEASURE_START(collect_derivatives);
    // Step 2. Collect valid Edwards y derivatives
    const std::size_t n_valid_outputs = std::count(valid_outputs.begin(), valid_outputs.end(), True);
    const std::size_t n_valid_leaf_points = n_valid_outputs * LEAF_TUPLE_POINTS;

    // Collecting [(1+y),(1-y),((1-y)*x)] for batch inversion
    std::unique_ptr<fe[]> one_plus_y_vec = std::make_unique<fe[]>(n_valid_leaf_points);
    std::unique_ptr<fe[]> fe_batch = std::make_unique<fe[]>(n_valid_leaf_points * 2);
    std::unique_ptr<fe[]> batch_inv_res = std::make_unique<fe[]>(n_valid_leaf_points * 2);

    std::size_t valid_i = 0, batch_i = 0;
    for (std::size_t i = 0; i < valid_outputs.size(); ++i)
    {
        if (!valid_outputs[i])
            continue;

        CHECK_AND_ASSERT_THROW_MES(n_valid_leaf_points > valid_i, "unexpected valid_i");

        auto& pl = pre_leaves.at(i);

        auto& O_derivatives = pl.O_derivatives;
        auto& I_derivatives = pl.I_derivatives;
        auto& C_derivatives = pl.C_derivatives;

        static_assert(LEAF_TUPLE_POINTS == 3, "unexpected n leaf tuple points");

        // TODO: avoid copying underlying (tried using pointer to pointers, but wasn't clean)
        memcpy(&one_plus_y_vec[valid_i++], &O_derivatives.one_plus_y, sizeof(fe));
        memcpy(&one_plus_y_vec[valid_i++], &I_derivatives.one_plus_y, sizeof(fe));
        memcpy(&one_plus_y_vec[valid_i++], &C_derivatives.one_plus_y, sizeof(fe));

        memcpy(&fe_batch[batch_i++], &O_derivatives.one_minus_y, sizeof(fe));
        memcpy(&fe_batch[batch_i++], &O_derivatives.one_minus_y_mul_x, sizeof(fe));

        memcpy(&fe_batch[batch_i++], &I_derivatives.one_minus_y, sizeof(fe));
        memcpy(&fe_batch[batch_i++], &I_derivatives.one_minus_y_mul_x, sizeof(fe));

        memcpy(&fe_batch[batch_i++], &C_derivatives.one_minus_y, sizeof(fe));
        memcpy(&fe_batch[batch_i++], &C_derivatives.one_minus_y_mul_x, sizeof(fe));
    }

    CHECK_AND_ASSERT_THROW_MES(n_valid_leaf_points == valid_i, "unexpected end valid_i");
    CHECK_AND_ASSERT_THROW_MES((n_valid_leaf_points * 2) == batch_i, "unexpected end batch_i");
    TIME_MEASURE_FINISH(collect_derivatives);

    TIME_MEASURE_START(batch_invert);
    // Step 3. Get batch inverse of all valid (1-y)'s and ((1-y)*x)'s
    // - Batch inversion is significantly faster than inverting 1 at a time
    fe_batch_invert(batch_inv_res.get(), fe_batch.get(), n_valid_leaf_points * 2);
    TIME_MEASURE_FINISH(batch_invert);

    TIME_MEASURE_START(get_selene_scalars);
    // Step 4. Multithreaded get Wei coordinates and convert to Selene scalars
    const std::size_t n_valid_leaf_elems = n_valid_outputs * LEAF_TUPLE_SIZE;
    flattened_leaves_out.resize(n_valid_leaf_elems);
    CHECK_AND_ASSERT_THROW_MES(flattened_leaves_out.size() == (2 * n_valid_leaf_points),
        "unexpected size of flattened leaves");

    const std::size_t DERIVATION_BATCH_SIZE = std::max<std::size_t>(1, (n_valid_leaf_points / n_threads));
    for (std::size_t i = 0; i < n_valid_leaf_points; i += DERIVATION_BATCH_SIZE)
    {
        const std::size_t end = std::min(n_valid_leaf_points, i + DERIVATION_BATCH_SIZE);
        tpool.submit(&waiter,
            [
                &batch_inv_res,
                &one_plus_y_vec,
                &flattened_leaves_out,
                i,
                end
            ]()
            {
                std::size_t point_idx = i * 2;
                for (std::size_t j = i; j < end; ++j)
                {
                    crypto::ec_coord wei_x;
                    crypto::ec_coord wei_y;
                    fe_ed_derivatives_to_wei_x_y(
                        to_bytes(wei_x),
                        to_bytes(wei_y),
                        batch_inv_res[point_idx]/*inv_one_minus_y*/,
                        one_plus_y_vec[j],
                        batch_inv_res[point_idx + 1]/*inv_one_minus_y_mul_x*/
                    );

                    flattened_leaves_out[point_idx++] = tower_cycle::selene_scalar_from_bytes(wei_x);
                    flattened_leaves_out[point_idx++] = tower_cycle::selene_scalar_from_bytes(wei_y);
                }
            },
            true
        );
    }

    CHECK_AND_ASSERT_THROW_MES(waiter.wait(), "failed to convert outputs to wei coords");
    TIME_MEASURE_FINISH(get_selene_scalars);

    // Step 5. Set valid tuples to be stored in the db
    valid_outputs_out.reserve(n_valid_outputs);
    for (std::size_t i = 0; i < valid_outputs.size(); ++i)
    {
        if (!valid_outputs[i])
            continue;

        // We can derive leaf tuples from output pairs, so we store just the unified output in the db to save 32 bytes
        valid_outputs_out.emplace_back(std::move(new_outputs.at(i)));
    }
    CHECK_AND_ASSERT_THROW_MES(valid_outputs_out.size() == n_valid_outputs, "unexpected size of valid_outputs_out");