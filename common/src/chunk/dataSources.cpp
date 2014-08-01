#include "chunk/dataSources.hpp"

namespace om {
namespace datalayer {
template class Vector<coords::Chunk, uint32_t>;
template class IDataSource<coords::Chunk, chunk::UniqueValues>;
template class DataSourceHierarchy<coords::Chunk, chunk::UniqueValues>;
template class IDataSource<coords::Chunk, chunk::ChunkVar>;
template class DataSourceHierarchy<coords::Chunk, chunk::ChunkVar>;
}
}  // namespace om::chunk::