#ifndef OM_TILE_CACHE_SEGMENTATION_HPP
#define OM_TILE_CACHE_SEGMENTATION_HPP

#include "common/om.hpp"
#include "system/cache/omThreadedCache.h"
#include "utility/omLockedObjects.h"
#include "tiles/omTileCoord.h"
#include "tiles/omTile.h"

class OmTileCacheSegmentation : private OmThreadedCache<OmTileCoord, OmTilePtr>{
private:
    static const int NUM_THREADS = 3;
    static const int SLICE_SIZE_BYTES = 128*128*4;

public:
    OmTileCacheSegmentation()
        : OmThreadedCache<OmTileCoord, OmTilePtr>(om::TILE_CACHE,
                                                  "Segmentation Tiles",
                                                  NUM_THREADS,
                                                  om::THROTTLE,
                                                  om::DONT_FIFO,
                                                  SLICE_SIZE_BYTES)
    {}

    inline void Get(OmTilePtr& ptr, const OmTileCoord& key,
                    const om::Blocking isBlocking)
    {
        checkCacheFreshness(key);
        OmThreadedCache<OmTileCoord, OmTilePtr>::Get(ptr, key, isBlocking);
    }

    inline void RemoveDataCoord(const DataCoord& coord)
    {
        boost::shared_ptr<std::list<OmTileCoord> > tileCoordsToRemove =
            keysByDataCoord_.removeKey(coord);

        FOR_EACH(iter, *tileCoordsToRemove){
            Remove(*iter);
        }
    }

    void Clear()
    {
        keysByDataCoord_.clear();
        OmThreadedCache<OmTileCoord, OmTilePtr>::Clear();
    }

    inline void Prefetch(const OmTileCoord& key){
        OmThreadedCache<OmTileCoord, OmTilePtr>::Prefetch(key);
    }

private:
    zi::mutex mutex_;
    LockedMultiMap<DataCoord, OmTileCoord> keysByDataCoord_;
    LockedUint64 freshness_;

    inline void checkCacheFreshness(const OmTileCoord& key)
    {
        zi::guard g(mutex_); // locked so changes across structures are atomic

        if(key.getFreshness() > freshness_.get()){
            OmThreadedCache<OmTileCoord, OmTilePtr>::InvalidateCache();
            keysByDataCoord_.clear();
            freshness_.set(key.getFreshness());
        }
    }

    OmTilePtr HandleCacheMiss(const OmTileCoord& key)
    {
        if(key.getFreshness() != freshness_.get()){
            // key's freshness is too old or new
            return OmTilePtr();
        }

        keysByDataCoord_.insert(key.getDataCoord(), key);

        OmTile* tile = new OmTile(this, key);
        tile->LoadData();
        return OmTilePtr(tile);
    }

    friend class OmCacheManager;
};

#endif
