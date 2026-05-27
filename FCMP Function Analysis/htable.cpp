static constexpr std::size_t H_TABLE_SIZE = 64;
const std::vector<ge_cached>& H_TABLE()
{
    struct static_h_table
    {
        std::vector<ge_cached> h_table;

        static_h_table()
            : h_table()
        {
            h_table.resize(H_TABLE_SIZE);
            ge_p3_to_cached(&h_table.at(0), &ge_p3_H);
            ge_p3 H_bit_p3 = ge_p3_H;

            for (std::size_t i = 1; i < H_TABLE_SIZE; ++i)
            {
                ge_p1p1 H_bit_p1p1;
                ge_p3_dbl(&H_bit_p1p1, &H_bit_p3);
                ge_p1p1_to_p3(&H_bit_p3, &H_bit_p1p1);
                ge_p3_to_cached(&h_table.at(i), &H_bit_p3);
            }
        }
    };
    static const static_h_table out;
    return out.h_table;
}