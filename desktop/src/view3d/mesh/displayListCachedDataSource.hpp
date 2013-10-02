#pragma once

#include "mesh/dataSources.hpp"
#include "datalayer/cacheDataSource.hpp"
#include "mesh/displayListMeshDataSources.hpp"
#include "mesh/displayListMeshDataSource.hpp"
#include "datalayer/dataSourceHierarchy.hpp"

namespace om {
namespace mesh {

class DisplayListCachedDataSource : public DisplayListMeshDSH {
 private:
  const size_t toCache_;  // TODO: Make dynamic (Cache Manager)

 public:
  DisplayListCachedDataSource(VertexIndexMeshDS& rawMeshes)
      : DisplayListMeshDSH(), toCache_(10000) {

    Register<DisplayListMeshDataSource<VertexIndexMesh>>(0, rawMeshes);
    Register<datalayer::CacheDataSource<DisplayListMeshCoord,
                                        mesh::DisplayListMesh>>(0, toCache_);
  }

  virtual ~DisplayListCachedDataSource() {}
};
}
}  // om::mesh:;
