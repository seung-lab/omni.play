#pragma once
#include "precomp.h"

#include "mesh/omMesh.h"
#include "common/core.hpp"

namespace om {
class chunkCoord;
}
class OmMeshManager;

namespace om {
namespace v3d {
namespace mesh {

class CacheWrapper {
 private:
  std::unique_ptr<OmMeshManager> meshManager_;

 public:
  CacheWrapper(om::common::ID segmentationID);
  ~CacheWrapper();

  OmMeshPtr Get(const om::coords::Chunk&, const om::common::SegID);
};
}
}
}  // namespace
