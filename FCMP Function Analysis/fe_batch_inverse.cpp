void fe_batch_invert(fe* out, const fe* in, const int n) {
    if (n == 0) {
        return;
    }

    assert(out);
    assert(in);
#ifndef NDEBUG
    {
        const fe* pmin = out < in ? out : in;
        const fe* pmax = out > in ? out : in;
        assert(pmin + n < pmax);
    }
#endif
    fe_copy(out[0], in[0]);
    
    for (int i = 1; i < n; ++i) {
        fe_mul(out[i], out[i - 1], in[i]);
    }
    fe a;
    fe_invert(a, out[n - 1]);

    for (int i = n; i > 1; --i) {
        fe_mul(out[i - 1], a, out[i - 2]);
        fe_mul(a, a, in[i - 1]);
    }
    fe_copy(out[0], a);
}