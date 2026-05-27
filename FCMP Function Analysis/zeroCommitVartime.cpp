key zeroCommitVartime(xmr_amount amount) {
    const zero_commitment* begin = zero_commitments;
    const zero_commitment* end = zero_commitments + sizeof(zero_commitments)
        / sizeof(zero_commitments[0]);
    const zero_commitment value{ amount, rct::zero() };
    const auto it = std::lower_bound(begin, end, value,
        [](const zero_commitment& e0, const zero_commitment& e1)
        { return e0.amount < e1.amount; });
    if (it != end && it->amount == amount)
    {
        return it->commitment;
    }

    ge_p3 res_ge_p3 = get_G_p3();

    static_assert(sizeof(xmr_amount) * 8 == H_TABLE_SIZE,
        "unexpected size of h table");

    for (size_t i = 0; i < H_TABLE_SIZE; ++i)
    {
        if (amount & (xmr_amount(1) << i))
        {
            ge_p1p1 p1p1;
            ge_add(&p1p1, &res_ge_p3, &H_TABLE()[i]);
            ge_p1p1_to_p3(&res_ge_p3, &p1p1);
        }
    }
    rct::key res;
    ge_p3_tobytes(res.bytes, &res_ge_p3);
    return res;
}