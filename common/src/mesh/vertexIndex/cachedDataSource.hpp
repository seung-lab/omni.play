#pragma once

#include "datalayer/sizedCacheDataSource.hpp"
#include "mesh/vertexIndex/dataSource.h"
#include "mesh/dataSources.hpp"

namespace om {
namespace mesh {

class CachedDataSource : public mesh::VertexIndexMeshDSH {
 private:
  const size_t toCache_;  // TODO: Make dynamic (Cache Manager)

 public:
  CachedDataSource(file::path volumeRoot,
                   chunk::UniqueValuesDS* uniqueValuesSource)
      : mesh::VertexIndexMeshDSH(4), toCache_(1000) {  // 1000 * ~500KB = 500MB

    Register<mesh::VertexIndexDataSource>(0, volumeRoot, uniqueValuesSource);
    Register<
        datalayer::SizedCacheDataSource<coords::Mesh, mesh::VertexIndexMesh>>(
        0, toCache_);
  }

  virtual ~CachedDataSource() {}
};
}
}  // om::mesh:;
