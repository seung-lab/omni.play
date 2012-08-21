#include "system/cache/omCacheManager.h"
#include "tiles/omTile.h"
#include "view2d/omOnScreenTileCoords.h"
#include "view2d/omView2dState.hpp"
#include "viewGroup/omViewGroupState.h"
#include "volume/omChannel.h"
#include "volume/omFilter2d.h"
#include "volume/omMipVolume.h"

OmOnScreenTileCoords::OmOnScreenTileCoords(OmView2dState* state)
    : state_(state)
    , vol_(state->getVol())
    , viewType_(state->getViewType())
    , vgs_(state->getViewGroupState())
    , mipLevel_(state->getMipLevel())
    , tileCoordsAndLocations_(om::make_shared<OmTileCoordsAndLocations>())
{
    freshness_ = 0;
    if(SEGMENTATION == vol_->getVolumeType()){
        freshness_ = OmCacheManager::GetFreshness();
    }

    const Vector3i chunkVec = state_->Location().toDataCoord(vol_, mipLevel_).toChunkVec();
    dataDepth_ = state_->getViewTypeDepth(chunkVec);
}

std::ostream& operator<<(std::ostream &out, const OmOnScreenTileCoords& c)
{
    out << c.mipLevel_ << " - " << c.dataDepth_ << "\n";
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

int numChunks(om::chunkCoord min, om::chunkCoord max)
{
    return (max.Coordinate.x - min.Coordinate.x + 1) *
           (max.Coordinate.y - min.Coordinate.y + 1) *
           (max.Coordinate.z - min.Coordinate.z + 1);
}

void OmOnScreenTileCoords::doComputeCoordsAndLocations(const int depthOffset)
{
    // Make sure that the upper left and bottom right don't exceed the volume
    om::globalBbox bounds = vol_->Coords().GetDataExtent();

    Vector4i viewport = state_->Coords().getTotalViewport();
    om::globalCoord min = om::screenCoord(viewport.lowerLeftX, viewport.lowerLeftY, state_).toGlobalCoord();
    om::globalCoord max = om::screenCoord(viewport.width, viewport.height, state_).toGlobalCoord();

    om::globalBbox viewBounds(min, max);

    viewBounds.intersect(bounds);

    if (viewBounds.isEmpty()) {
    	return;
    }

    om::chunkCoord minChunk = om::globalCoord(viewBounds.getMin()).toChunkCoord(vol_, mipLevel_);
    om::chunkCoord maxChunk = om::globalCoord(viewBounds.getMax()).toChunkCoord(vol_, mipLevel_);

    // iterate over all chunks on the screen
    for (int x = minChunk.Coordinate.x; x <= maxChunk.Coordinate.x; x++)
    {
        for (int y = minChunk.Coordinate.y; y <= maxChunk.Coordinate.y; y++)
        {
            for (int z = minChunk.Coordinate.z; z <= maxChunk.Coordinate.z; z++)
            {
                om::chunkCoord coord(mipLevel_, x, y, z);
                computeTile(coord, depthOffset);
            }
        }
    }

}

void OmOnScreenTileCoords::computeTile(const om::chunkCoord& chunkCoord,
                                       const int depthOffset)
{
    if(!vol_->Coords().ContainsMipChunkCoord(chunkCoord))
    {
        std::cout << chunkCoord << std::endl;
        throw OmArgException("Bad Chunk Coord created.");
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
                                                   const om::chunkCoord & chunkCoord,
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

OmTileCoord OmOnScreenTileCoords::makeTileCoord(const om::chunkCoord& coord,
                                                const int depthOffset,
                                                OmMipVolume* vol,
                                                int freshness)
{
    return OmTileCoord(coord,
                       state_->getViewType(),
                       dataDepth_ + depthOffset,
                       vol,
                       freshness,
                       vgs_,
                       state_->getObjectType());
}

GLfloatBox OmOnScreenTileCoords::computeVertices(const om::chunkCoord& coord, const OmMipVolume* vol)
{
    om::dataBbox bounds = coord.chunkBoundingBox(vol);
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
