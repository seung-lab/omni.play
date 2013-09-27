#include "mesh/omMesh.h"
#include "system/cache/omMeshCache.h"
#include "mesh/omMeshManager.h"
#include <zi/system.hpp>

OmMeshCache::OmMeshCache(OmMeshManager* parent)
    : OmThreadedMeshCache(om::common::CacheGroup::MESH_CACHE, "Meshes")
    , meshMan_(parent)
{}

OmMeshPtr
OmMeshCache::HandleCacheMiss(const OmMeshCoord& coord)
{
    OmMeshPtr mesh = meshMan_->Produce(coord);

    //load data from disk
    mesh->Load();

    return mesh;
}
