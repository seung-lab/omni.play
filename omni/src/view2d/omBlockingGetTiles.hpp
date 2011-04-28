#ifndef OM_BLOCKING_GET_TILES_HPP
#define OM_BLOCKING_GET_TILES_HPP

#include "tiles/cache/omTileCache.h"
#include "tiles/omTileTypes.hpp"
#include "tiles/omTileImplTypes.hpp"
#include "threads/omTaskManager.hpp"
#include "zi/omMutex.h"

class OmBlockingGetTiles {
private:
    std::deque<OmTileAndVertices>& tilesToDraw_;

    zi::spinlock lock_;

    OmThreadPool pool_;
    zi::semaphore semaphore_;

public:
    OmBlockingGetTiles(std::deque<OmTileAndVertices>& tilesToDraw)
        : tilesToDraw_(tilesToDraw)
    {
        pool_.start(3);
    }

    void GetAll(OmTileCoordsAndLocationsPtr tileCoordsAndLocations)
    {
        const int64_t size = tileCoordsAndLocations->size();

        semaphore_.set(0);

        FOR_EACH(tileCL, *tileCoordsAndLocations)
        {
            pool_.push_back(
                zi::run_fn(
                    zi::bind(&OmBlockingGetTiles::getTile,
                             this, *tileCL)));
        }

        semaphore_.acquire(size);
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

        semaphore_.release(1);
    }
};

#endif
