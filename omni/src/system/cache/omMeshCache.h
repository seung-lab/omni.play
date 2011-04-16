#ifndef OM_MESH_CACHE_H
#define OM_MESH_CACHE_H

#include "system/cache/omThreadedMeshCache.h"
#include "mesh/omMeshTypes.h"
#include "mesh/omMeshCoord.h"

class OmMeshManager;

class OmMeshCache : public OmThreadedMeshCache {
public:
    OmMeshCache(OmMeshManager * parent);

    OmMeshPtr HandleCacheMiss(const OmMeshCoord &meshCoord);

private:
    OmMeshManager *const meshMan_;
};

#endif
