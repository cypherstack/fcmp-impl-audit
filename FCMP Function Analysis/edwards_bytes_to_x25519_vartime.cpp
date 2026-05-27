int edwards_bytes_to_x25519_vartime(unsigned char* xbytes, const unsigned char* s)
{
    fe Y;
    if (fe_frombytes_vartime(Y, s) != 0) {
        return -1;
    }

    fe Z;
    fe_1(Z);

    edwardsYZ_to_x25519(xbytes, Y, Z);

    return 0;
}