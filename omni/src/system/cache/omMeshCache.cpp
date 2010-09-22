#include "mesh/omMipMesh.h"
#include "system/cache/omMeshCache.h"
#include "mesh/omMipMeshManager.h"

OmMeshCache::OmMeshCache(OmMipMeshManager * parent)
	: OmThreadedCache<OmMipMeshCoord, OmMipMeshPtr>(VRAM_CACHE_GROUP,
							"Meshes")
	, mOmMipMeshManager(parent)
{
}

OmMipMeshPtr
OmMeshCache::HandleCacheMiss(const OmMipMeshCoord& coord)
{
	OmMipMeshPtr mesh = mOmMipMeshManager->AllocMesh(coord);

	//load data from disk
	try {
		mesh->Load();
	} catch (...) {
	}

	return mesh;
}
