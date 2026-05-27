void crypto_ops::unbiased_hash_to_ec(const unsigned char* preimage, const std::size_t length, ec_point& res) {
    uint8_t hash[64];
    blake2b(std::addressof(hash), 64, preimage, length, NULL, 0);

    ge_p2 first;
    ge_fromfe_frombytes_vartime(&first, reinterpret_cast<const unsigned char*>(&hash));
    ge_p1p1 first_p1p1;
    ge_mul8(&first_p1p1, &first);
    ge_p3 first_p3;
    ge_p1p1_to_p3(&first_p3, &first_p1p1);

    ge_p2 second;
    ge_fromfe_frombytes_vartime(&second, reinterpret_cast<const unsigned char*>(&hash) + 32);
    ge_p1p1 second_p1p1;
    ge_mul8(&second_p1p1, &second);
    ge_p3 second_p3;
    ge_p1p1_to_p3(&second_p3, &second_p1p1);
    ge_cached second_cached;
    ge_p3_to_cached(&second_cached, &second_p3);

    ge_p1p1 point;
    ge_add(&point, &first_p3, &second_cached);

    ge_p3 res_ge_p3;
    ge_p1p1_to_p3(&res_ge_p3, &point);
    ge_p3_tobytes(&res, &res_ge_p3);
}
