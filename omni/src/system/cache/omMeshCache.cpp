#include "system/cache/omMeshCache.h"
#include "mesh/omMipMeshManager.h"

OmMeshCache::OmMeshCache(OmMipMeshManager * parent) 
	: OmThreadedCache<OmMipMeshCoord, OmMipMesh>(VRAM_CACHE_GROUP)
	, mOmMipMeshManager(parent)
{
}
	
OmMipMesh* OmMeshCache::HandleCacheMiss(const OmMipMeshCoord & coord)
{
	//create mesh with this segment manager as cache
	OmMipMesh *mesh = mOmMipMeshManager->AllocMesh(coord);

	//load data from disk
	try {
		mesh->Load();
	} catch (...) {
	}

	//return mesh to cache
	return mesh;
}

