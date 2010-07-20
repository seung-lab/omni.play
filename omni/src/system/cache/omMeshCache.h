#ifndef OM_MESH_CACHE_H
#define OM_MESH_CACHE_H

#include "system/cache/omThreadedCache.h"
#include "mesh/omMipMeshCoord.h"
#include "mesh/omMipMesh.h"

class OmMipMeshManager;

class OmMeshCache : public OmThreadedCache<OmMipMeshCoord, OmMipMesh> {
public:
	OmMeshCache(OmMipMeshManager * parent);
	
	OmMipMesh* HandleCacheMiss(const OmMipMeshCoord &meshCoord);

private:
	OmMipMeshManager *const mOmMipMeshManager;
};

#endif
