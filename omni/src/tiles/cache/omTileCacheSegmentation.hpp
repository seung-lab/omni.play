#ifndef OM_TILE_CACHE_SEGMENTATION_HPP
#define OM_TILE_CACHE_SEGMENTATION_HPP

#include "common/om.hpp"
#include "tiles/cache/omThreadedTileCache.h"
#include "tiles/omTile.h"
#include "tiles/omTileCoord.h"
#include "utility/omLockedObjects.h"

class OmSegmentation;

class OmTileCacheSegmentation : public OmThreadedTileCache {
public:
    OmTileCacheSegmentation()
        : OmThreadedTileCache("Segmentation Tiles", 128*128*4)
    {}

    virtual ~OmTileCacheSegmentation()
    {}

    void Load(OmSegmentation*)
    {}

    virtual void Get(OmTilePtr& ptr, const OmTileCoord& key,
                     const om::Blocking isBlocking)
    {
        checkCacheFreshness(key);
        OmThreadedTileCache::Get(ptr, key, isBlocking);
    }

private:
    LockedUint64 freshness_;

    inline void checkCacheFreshness(const OmTileCoord& key)
    {
        if(key.getFreshness() > freshness_.get())
        {
            OmThreadedTileCache::InvalidateCache();
            freshness_.set(key.getFreshness());
        }
    }

    friend class OmCacheManager;
};

#endif
