#include "mesh/omMipMesh.h"
#include "system/cache/omMeshCache.h"
#include "mesh/omMipMeshManager.h"
#include <zi/system.hpp>

OmMeshCache::OmMeshCache(OmMipMeshManager* parent)
    : OmThreadedCache<OmMipMeshCoord, OmMipMeshPtr>(om::MESH_CACHE,
                                                    "Meshes",
                                                    zi::system::cpu_count,
                                                    om::THROTTLE,
                                                    om::FIFO,
                                                    0)
    , meshMan_(parent)
{}

OmMipMeshPtr
OmMeshCache::HandleCacheMiss(const OmMipMeshCoord& coord)
{
    OmMipMeshPtr mesh = meshMan_->Produce(coord);

    //load data from disk
    mesh->Load();

    return mesh;
}
