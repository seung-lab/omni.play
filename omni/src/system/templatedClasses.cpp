#include "system/cache/omThreadedCache.cpp"

#include "volume/omRawChunk.hpp"

#include "mesh/omMipMeshCoord.h"
#include "mesh/omMeshTypes.h"
#include "mesh/omMipMesh.h"

#include "tiles/omTileCoord.h"
#include "tiles/omTile.h"

template class OmThreadedCache<OmMipChunkCoord,
					  boost::shared_ptr<OmRawChunk<int8_t> > >;
template class OmThreadedCache<OmMipChunkCoord,
					  boost::shared_ptr<OmRawChunk<uint8_t> > >;
template class OmThreadedCache<OmMipChunkCoord,
					  boost::shared_ptr<OmRawChunk<int32_t> > >;
template class OmThreadedCache<OmMipChunkCoord,
					  boost::shared_ptr<OmRawChunk<uint32_t> > >;
template class OmThreadedCache<OmMipChunkCoord,
					  boost::shared_ptr<OmRawChunk<float> > >;
template class OmThreadedCache<OmMipMeshCoord, OmMipMeshPtr>;
template class OmThreadedCache<OmTileCoord, OmTilePtr>;
