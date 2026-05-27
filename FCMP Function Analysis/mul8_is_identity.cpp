bool mul8_is_identity(const ge_p3& point) {
    ge_p2 point_ge_p2;
    ge_p3_to_p2(&point_ge_p2, &point);
    ge_p1p1 point_mul8;
    ge_mul8(&point_mul8, &point_ge_p2);
    ge_p3 point_mul8_p3;
    ge_p1p1_to_p3(&point_mul8_p3, &point_mul8);
    return ge_p3_is_point_at_infinity_vartime(&point_mul8_p3);
}