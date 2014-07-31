#include "datalayer/memMappedFile.hpp"
#include "mesh/vertexIndex/chunkTypes.hpp"

namespace om {
namespace datalayer {

template class MemMappedFile<mesh::DataEntry>;
template class MemMappedFile<char>;
template class MemMappedFile<int8_t>;
template class MemMappedFile<uint8_t>;
template class MemMappedFile<int32_t>;
template class MemMappedFile<uint32_t>;
template class MemMappedFile<float>;

}  // namespace datalayer
}  // namespace om
