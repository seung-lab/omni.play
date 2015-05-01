#pragma once
#include "precomp.h"

#include "system/cache/omThreadedMeshCache.h"
#include "view3d/mesh/omMeshTypes.h"

class OmMeshManager;

class OmMeshCache : public OmThreadedMeshCache {
 public:
  OmMeshCache(OmMeshManager *parent);

  OmMeshPtr HandleCacheMiss(const om::coords::Mesh &meshCoord);

 private:
  OmMeshManager *const meshMan_;
};
