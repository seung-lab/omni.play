#pragma once

#include "common/common.h"
#include "datalayer/dataSource.hpp"
#include "datalayer/vector.hpp"
#include "chunk/chunk.hpp"

namespace om {
namespace datalayer {
template <typename TKey, typename TValue> class DataSourceHierarchy;
}
namespace chunk {

typedef datalayer::Vector<coords::Chunk, uint32_t> UniqueValues;
typedef datalayer::IDataSource<coords::Chunk, UniqueValues> UniqueValuesDS;
typedef datalayer::DataSourceHierarchy<coords::Chunk, UniqueValues>
    UniqueValuesDSH;

typedef datalayer::IDataSource<coords::Chunk, ChunkVar> ChunkDS;
typedef datalayer::DataSourceHierarchy<coords::Chunk, ChunkVar> ChunkDSH;
}
}  // namespace om::chunk::
