#pragma once

#include "common/common.h"
#include "mesh/vertexIndex/mesh.h"

namespace om {
namespace datalayer {
template <typename TKey, typename TValue> class DataSourceHierarchy;
}
namespace mesh {

typedef datalayer::IDataSource<coords::Mesh, VertexIndexMesh> VertexIndexMeshDS;
typedef datalayer::DataSourceHierarchy<coords::Mesh, VertexIndexMesh>
    VertexIndexMeshDSH;
}
}  // namespace om::mesh::