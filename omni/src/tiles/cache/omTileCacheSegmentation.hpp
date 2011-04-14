#ifndef OM_TILE_CACHE_SEGMENTATION_HPP
#define OM_TILE_CACHE_SEGMENTATION_HPP

#include "common/om.hpp"
#include "system/cache/omThreadedCache.h"
#include "tiles/omTile.h"
#include "tiles/omTileCoord.h"
#include "utility/omLockedObjects.h"

class OmTileCacheSegmentation : private OmThreadedCache<OmTileCoord, OmTilePtr> {
public:
    OmTileCacheSegmentation()
        : OmThreadedCache<OmTileCoord, OmTilePtr>(om::TILE_CACHE,
                                                  "Segmentation Tiles",
                                                  om::DONT_THROTTLE)
    {}

    inline void Get(OmTilePtr& ptr, const OmTileCoord& key,
                    const om::Blocking isBlocking)
    {
        checkCacheFreshness(key);
        OmThreadedCache<OmTileCoord, OmTilePtr>::Get(ptr, key, isBlocking);
    }

    inline void GetDontQueue(OmTilePtr& ptr, const OmTileCoord& key)
    {
        // no need to check freshness--method used for fetching downssampled images
        OmThreadedCache<OmTileCoord, OmTilePtr>::GetDontQueue(ptr, key);
    }

    inline void BlockingCreate(OmTilePtr& tile, const OmTileCoord& key)
    {
        OmThreadedCache<OmTileCoord, OmTilePtr>::BlockingCreate(tile, key);
    }

    void Clear(){
        OmThreadedCache<OmTileCoord, OmTilePtr>::Clear();
    }

    inline void Prefetch(const OmTileCoord& key){
        OmThreadedCache<OmTileCoord, OmTilePtr>::Prefetch(key);
    }

    inline void ClearFetchQueue(){
        OmThreadedCache<OmTileCoord, OmTilePtr>::ClearFetchQueue();
    }

private:
    zi::spinlock mutex_;
    LockedUint64 freshness_;

    inline void checkCacheFreshness(const OmTileCoord& key)
    {
        zi::guard g(mutex_); // locked so changes across structures are atomic

        if(key.getFreshness() > freshness_.get())
        {
            OmThreadedCache<OmTileCoord, OmTilePtr>::InvalidateCache();
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
