SeleneScalar selene_scalar_from_bytes(const crypto::ec_coord& bytes)
{
    SeleneScalar selene_scalar;
    int r = ::selene_scalar_from_bytes(to_bytes(bytes), &selene_scalar);
    CHECK_FFI_RES;
    return selene_scalar;
}