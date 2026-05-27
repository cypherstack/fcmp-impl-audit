int fe_equals(const fe a, const fe b) {
    unsigned char a_bytes[32];
    unsigned char b_bytes[32];
    fe_tobytes(a_bytes, a);
    fe_tobytes(b_bytes, b);

    int r = 1;
    for (int i = 0; i < 32; ++i) {
        r &= a_bytes[i] == b_bytes[i];
    }
    return r;
}