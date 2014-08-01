#include "chunk/voxelGetter.hpp"

namespace om {
namespace chunk {
template class Voxels<int8_t>;
template class Voxels<uint8_t>;
template class Voxels<int32_t>;
template class Voxels<uint32_t>;
template class Voxels<float>;
}
}  // namespace om::chunk::
