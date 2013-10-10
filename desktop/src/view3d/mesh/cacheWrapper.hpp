#pragma once

#include "mesh/omMesh.h"
#include "common/core.hpp"

namespace om {
class chunkCoord;
}
class OmMeshManager;
class OmMeshCoord;

namespace om {
namespace v3d {
namespace mesh {

class CacheWrapper {
 private:
  std::unique_ptr<OmMeshManager> meshManager_;

 public:
  CacheWrapper(om::common::ID segmentationID);
  ~CacheWrapper();

  OmMeshPtr Get(const om::chunkCoord&, const om::common::SegID);
};

}
}
}  // namespace
