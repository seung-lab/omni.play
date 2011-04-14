#ifndef OM_CALC_TILE_COORDS_DOWNSAMPLED_HPP
#define OM_CALC_TILE_COORDS_DOWNSAMPLED_HPP

#include "tiles/cache/omTileCache.h"
#include "tiles/omTileTypes.hpp"
#include "utility/dataWrappers.h"
#include "view2d/omView2dConverters.hpp"
#include "zi/omUtility.h"

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

    void TryDownsample(const OmTileCoordAndVertices& tcv,
                       std::deque<OmTileAndVertices>& tilesToDraw)
    {
        OmMipVolume* vol = tcv.tileCoord.getVolume();
        const int rootMipLevel = vol->Coords().GetRootMipLevel();

        const int initialMipLevel = tcv.tileCoord.getLevel();
        const DataCoord& initialDataCoord = tcv.tileCoord.getDataCoord();

        OmTileCoord oldTileCoord = tcv.tileCoord;
        const GLfloatBox& vertices = tcv.vertices;

        for(int mipLevel = initialMipLevel + 1; mipLevel <= rootMipLevel; ++mipLevel)
        {
            const int newTileSize = 128 * om::pow2int(mipLevel);

            const DataCoord& oldDataCoord = oldTileCoord.getDataCoord();

            const DataCoord newDataCoord = makeNewDataCoord(oldDataCoord, newTileSize);

            OmTileCoord downsampledTileCoord(mipLevel,
                                             newDataCoord,
                                             oldTileCoord.getVolume(),
                                             oldTileCoord.getFreshness(),
                                             oldTileCoord.getViewGroupState(),
                                             oldTileCoord.getViewType(),
                                             oldTileCoord.getSegmentColorCacheType());

            OmTilePtr downsampledTile;
            OmTileCache::GetDontQueue(drawer_, downsampledTile, downsampledTileCoord);

            if(downsampledTile)
            {
                OmTileAndVertices tv = {downsampledTile,
                                        vertices,
                                        getTextureVertices(initialDataCoord, initialMipLevel,
                                                           mipLevel)
                };

                tilesToDraw.push_back(tv);

                return;
            }

            oldTileCoord = downsampledTileCoord;
        }
    }

    DataCoord makeNewDataCoord(const DataCoord& oldDataCoord, const int newTileSize)
    {
        const Vector2i ptsInPlane = OmView2dConverters::Get2PtsInPlane(oldDataCoord, viewType_);

        const int depth = OmView2dConverters::GetViewTypeDepth(oldDataCoord, viewType_);

        return OmView2dConverters::MakeViewTypeVector3(ROUNDDOWN(ptsInPlane.x, newTileSize),
                                                       ROUNDDOWN(ptsInPlane.y, newTileSize),
                                                       depth,
                                                       viewType_);
    }

    TextureVectices getTextureVertices(const DataCoord& oldDC, const int initialMipLevel,
                                       const int curMipLevel)
    {
        const Vector2i initialPtsInPlane = OmView2dConverters::Get2PtsInPlane(oldDC, viewType_);
        const int initialTileSize = 128 * om::pow2int(initialMipLevel);
        const int finalTileSize = 128 * om::pow2int(curMipLevel);

        const Vector2f pts(initialPtsInPlane.x % finalTileSize,
                           initialPtsInPlane.y % finalTileSize);

        const Vector2f textureRet = pts / static_cast<float>(finalTileSize);

        const float inc = static_cast<float>(initialTileSize) / static_cast<float>(finalTileSize);

        TextureVectices ret;

        // switch(viewType_){
        // case XY_VIEW:
        // case XZ_VIEW:
            ret.upperLeft.x = textureRet.x;
            ret.upperLeft.y = textureRet.y + inc;
            ret.lowerRight.x = textureRet.x + inc;
            ret.lowerRight.y = textureRet.y;
            // break;
        // case ZY_VIEW:
        //     ret.upperLeft.x = textureRet.x;
        //     ret.upperLeft.y = textureRet.y + inc;
        //     ret.lowerRight.x = textureRet.x + inc;
        //     ret.lowerRight.y = textureRet.y;
        //     break;
        // default:
        //     throw OmArgException("unknown viewType_");
        // }

        // std::cout << "returning: " << ret << "\n";

        return ret;
    }
};

#endif
