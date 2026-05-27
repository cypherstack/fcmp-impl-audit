void ge_p3_to_x25519(unsigned char* xbytes, const ge_p3* h)
{
	edwardsYZ_to_x25519(xbytes, h->Y, h->Z);
}
