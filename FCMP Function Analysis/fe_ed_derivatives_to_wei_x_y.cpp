void fe_ed_derivatives_to_wei_x_y(unsigned char* wei_x, unsigned char* wei_y, const fe inv_one_minus_y, const fe one_plus_y, const fe inv_one_minus_y_mul_x)
{
	fe_ed_derivatives_to_wei_x(wei_x, inv_one_minus_y, one_plus_y);

	// c*(1+y)
	fe fe_c_mul_one_plus_y;
	fe_mul(fe_c_mul_one_plus_y, fe_c, one_plus_y);

	// wei y = c * (1+y) * (1/((1-y)*x))
	fe wei_y_fe;
	fe_mul(wei_y_fe, fe_c_mul_one_plus_y, inv_one_minus_y_mul_x);
	fe_tobytes(wei_y, wei_y_fe);
}