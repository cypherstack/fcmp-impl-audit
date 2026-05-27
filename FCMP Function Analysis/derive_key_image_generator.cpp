void crypto_ops::derive_key_image_generator(const public_key& pub, const bool biased, ec_point& ki_gen) {
    if (biased)
        biased_derive_key_image_generator(pub, ki_gen);
    else
        unbiased_derive_key_image_generator(pub, ki_gen);
}