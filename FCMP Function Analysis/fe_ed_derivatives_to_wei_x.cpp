static void fe_ed_derivatives_to_wei_x(unsigned char* wei_x, const fe inv_one_minus_y, const fe one_plus_y)
{
	// (1/(1-y))*(1+y)
	fe inv_one_minus_y_mul_one_plus_y;
	fe_mul(inv_one_minus_y_mul_one_plus_y, inv_one_minus_y, one_plus_y);

	// wei x = (1/(1-y))*(1+y) + (A/3)
	fe wei_x_fe;
	fe_add(wei_x_fe, inv_one_minus_y_mul_one_plus_y, fe_a_inv_3);
	fe_tobytes(wei_x, wei_x_fe);
}