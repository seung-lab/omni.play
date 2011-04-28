#ifndef OM_BLOCKING_GET_TILES_HPP
#define OM_BLOCKING_GET_TILES_HPP

#include "tiles/cache/omTileCache.h"
#include "tiles/omTileTypes.hpp"
#include "tiles/omTileImplTypes.hpp"
#include "zi/omMutex.h"

class OmBlockingGetTiles {
private:
    std::deque<OmTileAndVertices>& tilesToDraw_;

    zi::spinlock lock_;

public:
    OmBlockingGetTiles(std::deque<OmTileAndVertices>& tilesToDraw)
        : tilesToDraw_(tilesToDraw)
    {}

    void GetAll(OmTileCoordsAndLocationsPtr tileCoordsAndLocations)
    {
        zi::for_each(tileCoordsAndLocations->begin(),
                     tileCoordsAndLocations->end(),
                     boost::bind(&OmBlockingGetTiles::getTile, this, _1));
    }

private:
    void getTile(const OmTileCoordAndVertices& tileCL)
    {
        static const TextureVectices defaultTextureVectices =
            { {0.f, 1.f}, {1.f, 0.f} };

        OmTilePtr tile;
        OmTileCache::BlockingCreate(tile, tileCL.tileCoord);

        OmTileAndVertices tv = {tile,
                                tileCL.vertices,
                                defaultTextureVectices};
        {
            zi::guard g(lock_);
            tilesToDraw_.push_back(tv);
        }
    }
};

#endif
