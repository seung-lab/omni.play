#ifndef OM_MESH_CACHE_H
#define OM_MESH_CACHE_H

#include "system/cache/omThreadedCache.h"
#include "mesh/omMeshTypes.h"
#include "mesh/omMipMeshCoord.h"

class OmMipMeshManager;

class OmMeshCache : public OmThreadedCache<OmMipMeshCoord, OmMipMeshPtr> {
public:
	OmMeshCache(OmMipMeshManager * parent);

	OmMipMeshPtr HandleCacheMiss(const OmMipMeshCoord &meshCoord);

private:
	OmMipMeshManager *const mOmMipMeshManager;
};

#endif
