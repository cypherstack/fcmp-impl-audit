static PreLeafTuple output_tuple_to_pre_leaf_tuple(const OutputTuple& o)
{
    TIME_MEASURE_NS_START(point_to_ed_derivatives_ns);

    const crypto::ec_point& O = (crypto::ec_point&)o.O;
    const crypto::ec_point& I = (crypto::ec_point&)o.I;
    const crypto::ec_point& C = (crypto::ec_point&)o.C;

    // TODO: we end up decompressing O and C twice, both in here and when checking the points for torsion. It's worth
    // checking how much of an impact that has on performance.
    PreLeafTuple plt;
    if (!fcmp_pp::point_to_ed_derivatives(O, plt.O_derivatives))
        throw std::runtime_error("failed to get ed derivatives from O");
    if (!fcmp_pp::point_to_ed_derivatives(I, plt.I_derivatives))
        throw std::runtime_error("failed to get ed derivatives from I");
    if (!fcmp_pp::point_to_ed_derivatives(C, plt.C_derivatives))
        throw std::runtime_error("failed to get ed derivatives from C");

    TIME_MEASURE_NS_FINISH(point_to_ed_derivatives_ns);

    LOG_PRINT_L3("point_to_ed_derivatives_ns: " << point_to_ed_derivatives_ns);

    return plt;
}