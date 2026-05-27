bool point_to_ed_derivatives(const crypto::ec_point& pub, EdDerivatives& ed_derivatives) {
    if (pub == crypto::EC_I)
        return false;
    // fe y;
    ge_p3 p3;
    if (ge_frombytes_vartime(&p3, to_bytes(pub)) != 0)
        return false;
    fe one;
    fe_1(one);
    // (1+y),(1-y)
    fe_add(ed_derivatives.one_plus_y, one, p3.Y);
    fe_sub(ed_derivatives.one_minus_y, one, p3.Y);
    // (1-y) * x
    fe_mul(ed_derivatives.one_minus_y_mul_x, ed_derivatives.one_minus_y, p3.X);
    return true;
}