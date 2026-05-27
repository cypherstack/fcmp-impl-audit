int fe_reduce_vartime(fe reduced_f, const fe f)
{
	unsigned char f_bytes[32];
	fe_tobytes(f_bytes, f);
	return fe_frombytes_vartime(reduced_f, f_bytes);
}