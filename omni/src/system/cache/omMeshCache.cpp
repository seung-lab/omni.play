#include "mesh/omMipMesh.h"
#include "system/cache/omMeshCache.h"
#include "mesh/omMipMeshManager.h"

static const int NUM_THREADS = 3;

OmMeshCache::OmMeshCache(OmMipMeshManager * parent)
	: OmThreadedCache<OmMipMeshCoord, OmMipMeshPtr>(VRAM_CACHE_GROUP,
													"Meshes",
													NUM_THREADS)
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
