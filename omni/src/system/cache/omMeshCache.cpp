#include "mesh/omMipMesh.h"
#include "system/cache/omMeshCache.h"
#include "mesh/omMipMeshManager.h"
#include <zi/system.hpp>

OmMeshCache::OmMeshCache(OmMipMeshManager* parent)
    : OmThreadedCache<OmMipMeshCoord, OmMipMeshPtr>(om::MESH_CACHE,
                                                    "Meshes",
                                                    zi::system::cpu_count,
                                                    om::THROTTLE,
                                                    om::FIFO)
    , mOmMipMeshManager(parent)
{}

OmMipMeshPtr
OmMeshCache::HandleCacheMiss(const OmMipMeshCoord& coord)
{
    OmMipMeshPtr mesh = mOmMipMeshManager->Produce(coord);

    //load data from disk
    mesh->Load();

    return mesh;
}
