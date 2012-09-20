#pragma once

#include "tiles/cache/omTileCache.h"
#include "tiles/omTileTypes.hpp"
#include "utility/dataWrappers.h"
#include "view2d/omView2dConverters.hpp"
#include "zi/omUtility.h"

class OmCalcTileCoordsDownsampled {
private:
    const ViewType viewType_;

public:
    OmCalcTileCoordsDownsampled(const ViewType viewType)
        : viewType_(viewType)
    {}

    void TryDownsample(const OmTileCoordAndVertices& tcv,
                       std::deque<OmTileAndVertices>& tilesToDraw)
    {
        OmMipVolume* vol = tcv.tileCoord.getVolume();
        const int rootMipLevel = vol->Coords().GetRootMipLevel();
        OmTileCoord tileCoord = tcv.tileCoord;

        while(tileCoord.getCoord().getLevel() < rootMipLevel)
        {
            tileCoord = tileCoord.Downsample();

            OmTilePtr downsampledTile;
            OmTileCache::GetDontQueue(downsampledTile, tileCoord);

            if(downsampledTile)
            {
                OmTileAndVertices tv = {
                	downsampledTile,
                    tcv.vertices,
                    getTextureVertices(tcv.tileCoord.getCoord(),
                    				   tileCoord.getCoord().getLevel())
                };

                tilesToDraw.push_back(tv);
                return;
            }
        }

        OmTileCache::QueueUp(tileCoord);
    }

    TextureVectices getTextureVertices(const om::chunkCoord& old, const int curMipLevel)
    {
    	Vector2i chunkCoordsInPlane = OmView2dConverters::Get2PtsInPlane(old.Coordinate, viewType_);
    	int mipDiff = om::math::pow2int(curMipLevel - old.getLevel());
    	float inc = 1.0f / mipDiff;
    	Vector2f textureRet(chunkCoordsInPlane.x % mipDiff * inc,
    	                    chunkCoordsInPlane.y % mipDiff * inc);

		TextureVectices ret;

        ret.upperLeft.x = textureRet.x;
        ret.upperLeft.y = textureRet.y + inc;
        ret.lowerRight.x = textureRet.x + inc;
        ret.lowerRight.y = textureRet.y;

        return ret;
    }
};

