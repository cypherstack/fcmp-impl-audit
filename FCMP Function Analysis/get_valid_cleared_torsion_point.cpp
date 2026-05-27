bool get_valid_torsion_cleared_point(const crypto::ec_point& point, crypto::ec_point& torsion_cleared_out) {
    ge_p3 p3;
    if (ge_frombytes_vartime(&p3, to_bytes(point)) != 0)
        return false;
    if (mul8_is_identity(p3))
        return false;
    torsion_cleared_out = fcmp_pp::clear_torsion(p3);
    if (torsion_cleared_out == crypto::EC_I)
        return false;
    return true;
}