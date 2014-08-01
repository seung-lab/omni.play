#pragma once

#include "common/common.h"
#include "datalayer/dataSource.hpp"
#include "datalayer/vector.hpp"
#include "datalayer/dataSourceHierarchy.hpp"
#include "chunk/chunk.hpp"

namespace om {
namespace chunk {

typedef datalayer::Vector<coords::Chunk, uint32_t> UniqueValues;
typedef datalayer::IDataSource<coords::Chunk, UniqueValues> UniqueValuesDS;
typedef datalayer::DataSourceHierarchy<coords::Chunk, UniqueValues>
    UniqueValuesDSH;

typedef datalayer::IDataSource<coords::Chunk, ChunkVar> ChunkDS;
typedef datalayer::DataSourceHierarchy<coords::Chunk, ChunkVar> ChunkDSH;
}
namespace datalayer {
extern template class Vector<coords::Chunk, uint32_t>;
extern template class IDataSource<coords::Chunk, chunk::UniqueValues>;
extern template class DataSourceHierarchy<coords::Chunk, chunk::UniqueValues>;
extern template class IDataSource<coords::Chunk, chunk::ChunkVar>;
extern template class DataSourceHierarchy<coords::Chunk, chunk::ChunkVar>;
}

}  // namespace om::chunk::
