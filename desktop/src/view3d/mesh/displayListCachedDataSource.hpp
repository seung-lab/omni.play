#pragma once

#include "mesh/dataSources.hpp"
#include "datalayer/cacheDataSource.hpp"
#include "view3d/mesh/displayListMeshDataSources.hpp"
#include "view3d/mesh/displayListMeshDataSource.hpp"
#include "datalayer/dataSourceHierarchy.hpp"

namespace om {
namespace mesh {

class DisplayListCachedDataSource : public DisplayListMeshDSH {
 private:
  const size_t toCache_;  // TODO: Make dynamic (Cache Manager)

 public:
  // Needs to be dynamic based on GPU memory instead of CPU memory.
  DisplayListCachedDataSource(VertexIndexMeshDS& rawMeshes)
      : DisplayListMeshDSH(), toCache_(1000) {

    Register<DisplayListMeshDataSource<VertexIndexMesh>>(0, rawMeshes);
    Register<datalayer::CacheDataSource<DisplayListMeshCoord,
                                        mesh::DisplayListMesh>>(0, toCache_);
  }

  virtual ~DisplayListCachedDataSource() {}
};
}
}  // om::mesh:;
