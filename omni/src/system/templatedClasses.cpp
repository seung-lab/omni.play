#include "system/cache/omThreadedCache.cpp"

#include "mesh/omMipMeshCoord.h"
#include "mesh/omMeshTypes.h"
#include "mesh/omMipMesh.h"

#include "tiles/omTileCoord.h"
#include "tiles/omTile.h"

template class OmThreadedCache<OmMipMeshCoord, OmMipMeshPtr>;
template class OmThreadedCache<OmTileCoord, OmTilePtr>;
