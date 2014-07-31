#include "chunk/rawChunkSlicer.hpp"

namespace om {
namespace chunk {

template class rawChunkSlicer<int8_t>;
template class rawChunkSlicer<uint8_t>;
template class rawChunkSlicer<int32_t>;
template class rawChunkSlicer<uint32_t>;
template class rawChunkSlicer<float>;

}  // namespace chunks
}  // namespace om
