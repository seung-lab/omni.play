#include "chunk/chunk.hpp"

namespace om {
namespace chunk {

template class Chunk<int8_t>;
template class Chunk<uint8_t>;
template class Chunk<int32_t>;
template class Chunk<uint32_t>;
template class Chunk<float>;
}
}

template class indexed_iterator<om::chunk::Chunk<int8_t>, int8_t>;
template class indexed_iterator<om::chunk::Chunk<uint8_t>, uint8_t>;
template class indexed_iterator<om::chunk::Chunk<int32_t>, int32_t>;
template class indexed_iterator<om::chunk::Chunk<uint32_t>, uint32_t>;
template class indexed_iterator<om::chunk::Chunk<float>, float>;
template class indexed_iterator<om::chunk::Chunk<const int8_t>, const int8_t>;
template class indexed_iterator<om::chunk::Chunk<const uint8_t>, const uint8_t>;
template class indexed_iterator<om::chunk::Chunk<const int32_t>, const int32_t>;
template class indexed_iterator<om::chunk::Chunk<const uint32_t>,
                                const uint32_t>;
template class indexed_iterator<om::chunk::Chunk<const float>, const float>;