#ifndef OM_CALC_TILE_COORDS_DOWNSAMPLED_HPP
#define OM_CALC_TILE_COORDS_DOWNSAMPLED_HPP

#include "tiles/omTileTypes.hpp"
#include "tiles/cache/omTileCache.h"
#include "zi/omUtility.h"
#include "utility/dataWrappers.h"

class OmCalcTileCoordsDownsampled {
private:
    const ViewType viewType_;
    OmTileDrawer *const drawer_;

public:
    OmCalcTileCoordsDownsampled(const ViewType viewType,
                                OmTileDrawer* drawer)
        : viewType_(viewType)
        , drawer_(drawer)
    {}

    void TryDownsample(OmMipVolume* vol,
                       const OmTileCoord& oldCoord,
                       const GLfloatBox& vertices,
                       std::deque<OmTileAndVertices>& tilesToDraw)
    {
        const OmChunkCoord oldChunkCoord =
            vol->Coords().DataToMipCoord(oldCoord.getDataCoord(),
                                         oldCoord.getLevel());

        const OmChunkCoord newChunkCoord = oldChunkCoord.ParentCoord();

        if(!vol->Coords().ContainsMipChunkCoord(newChunkCoord)){
            return;
        }

        OmTileCoord downsampledTileCoord(newChunkCoord.Level,
                                         oldCoord.getDataCoord(),
                                         oldCoord.getVolume(),
                                         oldCoord.getFreshness(),
                                         oldCoord.getViewGroupState(),
                                         oldCoord.getViewType(),
                                         oldCoord.getSegmentColorCacheType());

        OmTilePtr downsampledTile;
        OmTileCache::Get(drawer_, downsampledTile, downsampledTileCoord,
                         om::NON_BLOCKING);

        if(downsampledTile){
            OmTileAndVertices tv = {downsampledTile,
                                    vertices,
                                    getTextureVertices(oldChunkCoord.Coordinate)};
            tilesToDraw.push_back(tv);
        } else {
            // TODO: work on this
            //tryDownsample( vol, downsampledTileCoord, om::NON_BLOCKING, vertices);
        }
    }

private:

    TextureVectices getTextureVertices(const DataCoord& coord)
    {
        TextureVectices ret;

        switch(viewType_){
        case XY_VIEW:
            if(coord.x & 1){
                ret.upperLeft.x = 0.5f;
                ret.lowerRight.x = 1.0f;
            } else {
                ret.upperLeft.x = 0.0f;
                ret.lowerRight.x = 0.5f;
            }

            if(coord.y & 1){
                ret.upperLeft.y = 1.0f;
                ret.lowerRight.y = 0.5f;
            } else {
                ret.upperLeft.y = 0.5f;
                ret.lowerRight.y = 0.0f;
            }
            break;

        case YZ_VIEW:

            if(coord.z & 1){
                ret.upperLeft.x = 0.0f;
                ret.lowerRight.x = 0.5f;
            } else {
                ret.upperLeft.x = 0.5f;
                ret.lowerRight.x = 1.0f;
            }

            if(coord.y & 1){
                ret.upperLeft.y = 1.0f;
                ret.lowerRight.y = 0.5f;
            } else {
                ret.upperLeft.y = 0.5f;
                ret.lowerRight.y = 0.0f;
            }
            break;

        case XZ_VIEW:
            if(coord.x & 1){
                ret.upperLeft.x = 0.5f;
                ret.lowerRight.x = 1.0f;
            } else {
                ret.upperLeft.x = 0.0f;
                ret.lowerRight.x = 0.5f;
            }

            if(coord.z & 1){
                ret.upperLeft.y = 1.0f;
                ret.lowerRight.y = 0.5f;
            } else {
                ret.upperLeft.y = 0.5f;
                ret.lowerRight.y = 0.0f;
            }
            break;
        }

        return ret;
    }
};

#endif
