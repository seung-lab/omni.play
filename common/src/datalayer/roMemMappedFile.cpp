#include "datalayer/roMemMappedFile.hpp"
#include "mesh/vertexIndex/chunkTypes.hpp"

namespace om {
namespace datalayer {

template class ROMemMappedFile<mesh::DataEntry>;
template class ROMemMappedFile<char>;
template class ROMemMappedFile<int8_t>;
template class ROMemMappedFile<uint8_t>;
template class ROMemMappedFile<int32_t>;
template class ROMemMappedFile<uint32_t>;
template class ROMemMappedFile<float>;

}  // namespace datalayer
}  // namespace om
