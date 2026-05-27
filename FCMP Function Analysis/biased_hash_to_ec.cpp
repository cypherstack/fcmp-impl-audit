static void biased_hash_to_ec(const public_key& key, ge_p3& res) {
    hash h;
    ge_p2 point;
    ge_p1p1 point2;
    cn_fast_hash(std::addressof(key), sizeof(public_key), h);
    ge_fromfe_frombytes_vartime(&point, reinterpret_cast<const unsigned char*>(&h));
    ge_mul8(&point2, &point);
    ge_p1p1_to_p3(&res, &point2);
}