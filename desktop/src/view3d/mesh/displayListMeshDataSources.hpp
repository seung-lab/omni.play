#pragma once
#include "precomp.h"

#include "datalayer/dataSource.hpp"

namespace om {
namespace datalayer {
template <typename TKey, typename TValue>
class DataSourceHierarchy;
}
namespace mesh {

struct DisplayListMeshCoord;
class DisplayListMesh;

typedef datalayer::IDataSource<DisplayListMeshCoord, DisplayListMesh>
    DisplayListMeshDS;
typedef datalayer::DataSourceHierarchy<DisplayListMeshCoord, DisplayListMesh>
    DisplayListMeshDSH;
}
}