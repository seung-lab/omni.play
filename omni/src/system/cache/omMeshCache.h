#ifndef OM_MESH_CACHE_H
#define OM_MESH_CACHE_H

#include "system/cache/omThreadedCache.h"
#include "mesh/omMeshTypes.h"
#include "mesh/omMeshCoord.h"

class OmMeshManager;

class OmMeshCache : public OmThreadedCache<OmMeshCoord, OmMeshPtr> {
public:
    OmMeshCache(OmMeshManager * parent);

    OmMeshPtr HandleCacheMiss(const OmMeshCoord &meshCoord);

private:
    OmMeshManager *const meshMan_;
};

#endif
