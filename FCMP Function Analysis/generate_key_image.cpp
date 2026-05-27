void crypto_ops::generate_key_image(const public_key& pub, const secret_key& sec, key_image& image) {
    ge_p3 point;
    ge_p2 point2;
    assert(sc_check(&sec) == 0);
    biased_hash_to_ec(pub, point);
    ge_scalarmult(&point2, &unwrap(sec), &point);
    ge_tobytes(&image, &point2);
}