#ifndef OM_TILE_CACHE_SEGMENTATION_HPP
#define OM_TILE_CACHE_SEGMENTATION_HPP

#include "common/om.hpp"
#include "tiles/cache/omThreadedTileCache.h"
#include "tiles/omTile.h"
#include "tiles/omTileCoord.h"
#include "utility/omLockedObjects.h"

class OmTileCacheSegmentation : public OmThreadedTileCache {
public:
    OmTileCacheSegmentation()
        : OmThreadedTileCache("Segmentation Tiles")
    {}

    virtual void Get(OmTilePtr& ptr, const OmTileCoord& key,
                     const om::Blocking isBlocking)
    {
        checkCacheFreshness(key);
        OmThreadedTileCache::Get(ptr, key, isBlocking);
    }

private:
    zi::spinlock mutex_;
    LockedUint64 freshness_;

    inline void checkCacheFreshness(const OmTileCoord& key)
    {
        zi::guard g(mutex_); // locked so changes across structures are atomic

        if(key.getFreshness() > freshness_.get())
        {
            OmThreadedTileCache::InvalidateCache();
            freshness_.set(key.getFreshness());
        }
    }

    OmTilePtr HandleCacheMiss(const OmTileCoord& key)
    {
        OmTile* tile = new OmTile(this, key);
        tile->LoadData();
        return OmTilePtr(tile);
    }

    friend class OmCacheManager;
};

#endif
