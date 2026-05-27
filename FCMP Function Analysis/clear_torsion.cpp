crypto::ec_point clear_torsion(const ge_p3& point) {
    ge_p2 point_inv_8;
    ge_scalarmult(&point_inv_8, to_bytes(crypto::EC_INV_EIGHT), &point);
    ge_p1p1 point_inv_8_mul_8;
    ge_mul8(&point_inv_8_mul_8, &point_inv_8);
    ge_p3 torsion_cleared_point;
    ge_p1p1_to_p3(&torsion_cleared_point, &point_inv_8_mul_8);
    crypto::ec_point k_out;
    ge_p3_tobytes(to_bytes(k_out), &torsion_cleared_point);
    return k_out;
}