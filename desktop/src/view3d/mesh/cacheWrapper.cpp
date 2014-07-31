#include "system/cache/omThreadedMeshCache.h"
#include "mesh/omMeshTypes.h"
#include "mesh/omMeshManager.h"
#include "view3d/mesh/cacheWrapper.hpp"
#include "utility/segmentationDataWrapper.hpp"

namespace om {
namespace v3d {
namespace mesh {

CacheWrapper::CacheWrapper(om::common::ID segmentationID) {
  SegmentationDataWrapper sdw(segmentationID);
  auto* vol = sdw.GetSegmentation();
  meshManager_.reset(new OmMeshManager(vol, 1.0));
  meshManager_->Load();
}

CacheWrapper::~CacheWrapper() { meshManager_->CloseDownThreads(); }

OmMeshPtr CacheWrapper::Get(const om::coords::Chunk& coord,
                            const om::common::SegID segID) {
  OmMeshPtr mesh;
  om::coords::Mesh meshCoord(coord, segID);
  meshManager_->GetMesh(mesh, meshCoord, om::common::Blocking::NON_BLOCKING);
  return mesh;
}
}
}
}  // namespace
