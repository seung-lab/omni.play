#include "system/cache/omCacheManager.h"
#include "tiles/omTile.h"
#include "view2d/omOnScreenTileCoords.h"
#include "view2d/omView2dState.hpp"
#include "viewGroup/omViewGroupState.h"
#include "volume/omChannel.h"
#include "volume/omFilter2d.h"
#include "volume/omMipVolume.h"

OmOnScreenTileCoords::OmOnScreenTileCoords(OmView2dState* state, OmMipVolume* vol)
    : state_(state)
    , vol_(vol)
    , viewType_(state->getViewType())
    , vgs_(state->getViewGroupState())
    , mipLevel_(state->getMipLevel())
    , tileCoordsAndLocations_(om::make_shared<OmTileCoordsAndLocations>())
{
    freshness_ = 0;
    if(SEGMENTATION == vol_->getVolumeType()){
        freshness_ = OmCacheManager::GetFreshness();
    }
}

std::ostream& operator<<(std::ostream &out, const OmOnScreenTileCoords& c)
{
    out << c.mipLevel_ << std::endl;
    return out;
}

OmTileCoordsAndLocationsPtr
OmOnScreenTileCoords::ComputeCoordsAndLocations()
{
    return ComputeCoordsAndLocations(0);
}

OmTileCoordsAndLocationsPtr
OmOnScreenTileCoords::ComputeCoordsAndLocations(const int depthOffset)
{
    if(vol_->IsBuilt()){
		doComputeCoordsAndLocations(depthOffset);
    }

    OmTileCoordsAndLocationsPtr ret = tileCoordsAndLocations_;
    tileCoordsAndLocations_ = om::make_shared<OmTileCoordsAndLocations>();

    return ret;
}

int numChunks(om::coords::Chunk min, om::coords::Chunk max)
{
    return (max.Coordinate.x - min.Coordinate.x + 1) *
           (max.Coordinate.y - min.Coordinate.y + 1) *
           (max.Coordinate.z - min.Coordinate.z + 1);
}

void OmOnScreenTileCoords::doComputeCoordsAndLocations(const int depthOffset)
{
    om::coords::GlobalBbox bounds = vol_->Coords().Extent();
    om::coords::DataBbox dataBounds = bounds.ToDataBbox(vol_, mipLevel_);

    int dataDepth = state_->getViewTypeDepth(state_->Location().toDataCoord(vol_, mipLevel_));

    // Make sure that we aren't trying to fetch outside of the bounds of the data.
	int targetDepth = dataDepth + depthOffset;

	if (targetDepth < state_->getViewTypeDepth(dataBounds.getMin()) ||
	    targetDepth > state_->getViewTypeDepth(dataBounds.getMax())) {
		return;
	}

    // Make sure that the upper left and bottom right don't exceed the volume
    Vector4i viewport = state_->Coords().getTotalViewport();
    om::coords::Global min = om::screenCoord(viewport.lowerLeftX, viewport.lowerLeftY, state_).toGlobalCoord();
    om::coords::Global max = om::screenCoord(viewport.width, viewport.height, state_).toGlobalCoord();

    om::coords::GlobalBbox viewBounds(min, max);

    viewBounds.intersect(bounds);

    if (viewBounds.isEmpty()) {
    	return;
    }

    om::coords::Chunk minChunk = om::coords::Global(viewBounds.getMin()).toChunkCoord(vol_, mipLevel_);
    om::coords::Chunk maxChunk = om::coords::Global(viewBounds.getMax()).toChunkCoord(vol_, mipLevel_);

    // iterate over all chunks on the screen
    for (int x = minChunk.Coordinate.x; x <= maxChunk.Coordinate.x; x++)
    {
        for (int y = minChunk.Coordinate.y; y <= maxChunk.Coordinate.y; y++)
        {
            for (int z = minChunk.Coordinate.z; z <= maxChunk.Coordinate.z; z++)
            {
                om::coords::Chunk coord(mipLevel_, x, y, z);
                computeTile(coord, depthOffset);
            }
        }
    }

}

void OmOnScreenTileCoords::computeTile(const om::coords::Chunk& chunkCoord,
                                       const int depthOffset)
{
    if(!vol_->Coords().ContainsMipChunkCoord(chunkCoord)) {
        return; // Rounding errors can cause bad chunk coords to slip through.
    }

    if(depthOffset) // i.e. if we are pre-fetching
    {
        if(CHANNEL == vol_->getVolumeType())
        {
            OmChannel* chan = reinterpret_cast<OmChannel*>(vol_);

            const std::vector<OmFilter2d*> filters = chan->GetFilters();

            FOR_EACH(iter, filters){
                makeTileCoordFromFilter(*iter, chunkCoord, depthOffset);
            }
        }
    }

    OmTileCoordAndVertices pair = {makeTileCoord(chunkCoord, depthOffset, vol_, freshness_),
                                   computeVertices(chunkCoord, vol_) };


	// std::cout << chunkCoord << ":" << depthOffset << " - " << pair.tileCoord << std::endl;
    tileCoordsAndLocations_->push_back(pair);
}

void OmOnScreenTileCoords::makeTileCoordFromFilter(OmFilter2d* filter,
                                                   const om::coords::Chunk & chunkCoord,
                                                   const int depthOffset)
{
    const om::FilterType filterType = filter->FilterType();

    OmMipVolume* vol = NULL;
    int freshness = 0;

    if(om::OVERLAY_NONE == filterType){
        return;

    } else if (om::OVERLAY_CHANNEL == filterType) {
        vol = filter->GetChannel();

    } else {
        vol = filter->GetSegmentation();
        freshness = OmCacheManager::GetFreshness();
    }

    OmTileCoordAndVertices pair = { makeTileCoord(chunkCoord, depthOffset, vol, freshness),
                                    computeVertices(chunkCoord, vol) };

    tileCoordsAndLocations_->push_back(pair);
}

OmTileCoord OmOnScreenTileCoords::makeTileCoord(const om::coords::Chunk& coord,
                                                const int depthOffset,
                                                OmMipVolume* vol,
                                                int freshness)
{
	om::coords::Data loc = state_->Location().toDataCoord(vol, mipLevel_);
	int targetDepth = state_->getViewTypeDepth(loc) + depthOffset;
	state_->setViewTypeDepth(loc, targetDepth);

    return OmTileCoord(coord,
                       state_->getViewType(),
                       loc.toTileDepth(viewType_),
                       vol,
                       freshness,
                       vgs_,
                       state_->getObjectType());
}

GLfloatBox OmOnScreenTileCoords::computeVertices(const om::coords::Chunk& coord, const OmMipVolume* vol)
{
    om::coords::DataBbox bounds = coord.BoundingBox(vol);
    om::screenCoord min = bounds.getMin().toGlobalCoord().toScreenCoord(state_);
    om::screenCoord max = bounds.getMax().toGlobalCoord().toScreenCoord(state_);

    GLfloatBox glBox;
    glBox.lowerLeft.y  = min.y;
    glBox.upperRight.y = max.y;
    glBox.lowerLeft.x  = min.x;
    glBox.upperRight.x = max.x;


    glBox.lowerRight.x = glBox.upperRight.x;
    glBox.lowerRight.y = glBox.lowerLeft.y;

    glBox.upperLeft.x  = glBox.lowerLeft.x;
    glBox.upperLeft.y  = glBox.upperRight.y;

    return glBox;
}
