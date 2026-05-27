static PreLeafTuple output_to_pre_leaf_tuple(const OutputPair& output_pair)
{
    const auto o = output_to_tuple(output_pair);
    return output_tuple_to_pre_leaf_tuple(o);
}