#include "system/cache/omThreadedCache.cpp"

#include "volume/omMipChunk.h"
#include "volume/omVolumeTypes.hpp"

#include "mesh/omMipMeshCoord.h"
#include "mesh/omMeshTypes.h"
#include "mesh/omMipMesh.h"

#include "tiles/omTileCoord.h"
#include "tiles/omTile.h"

template class OmThreadedCache<OmMipChunkCoord, OmMipChunkPtr>;
template class OmThreadedCache<OmMipMeshCoord, OmMipMeshPtr>;
template class OmThreadedCache<OmTileCoord, OmTilePtr>;


#include "segment/lowLevel/omPagingPtrStore.cpp"
template class OmPagingPtrStore<OmSegment>;

