static void edwardsYZ_to_x25519(unsigned char* xbytes, const fe Y, const fe Z) {
	fe tmp0;
	fe tmp1;
	fe_add(tmp0, Z, Y);       
	fe_sub(tmp1, Z, Y);      
	fe_invert(tmp1, tmp1);    
	fe_mul(tmp0, tmp0, tmp1); 
	fe_tobytes(xbytes, tmp0); 
}