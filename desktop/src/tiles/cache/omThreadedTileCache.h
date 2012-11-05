#pragma once

#include "tiles/cache/omTileCacheThreadPool.hpp"
#include "common/om.hpp"
#include "system/cache/omCacheBase.h"
#include "system/cache/omCacheManager.h"
#include "system/cache/omLockedCacheObjects.hpp"
#include "tiles/cache/omTileCache.h"
#include "tiles/cache/omTilesToPrefetch.hpp"
#include "tiles/omTile.h"
#include "tiles/omTileTypes.hpp"
#include "utility/omLockedObjects.h"
#include "utility/omLockedPODs.hpp"

#include <zi/system.hpp>

/**
 *  Brett Warne - bwarne@mit.edu - 3/12/09
 */

class OmThreadedTileCache : public OmCacheBase {
private:
    typedef OmTileCoord key_t;
    typedef OmTilePtr ptr_t;

    const int bytesPerTile_;

    LockedCacheMap<key_t, ptr_t> cache_;
    LockedBool killingCache_;

    OmTileCacheThreadPool& threadPool_;

    struct OldCache {
        std::map<key_t,ptr_t> map;
        KeyMultiIndex<key_t> list;
    };
    LockedList<om::shared_ptr<OldCache> > cachesToClean_;

    OmTilesToPrefetch tilesToPrefetch_;

    LockedInt64 freshness_;

public:
    OmThreadedTileCache(const std::string& name, const int bytesPerTile)
        : OmCacheBase(name, om::TILE_CACHE)
        , bytesPerTile_(bytesPerTile)
        , threadPool_(OmTileCache::ThreadPool())
    {
        freshness_.set(0);

        OmCacheManager::AddCache(om::TILE_CACHE, this);
    }

    virtual ~OmThreadedTileCache()
    {
        OmCacheManager::RemoveCache(cacheGroup_, this);
    }

    virtual void Get(ptr_t& ptr, const key_t& key, const om::common::Blocking blocking)
    {
        if(freshness_.get() < key.getFreshness())
        {
            InvalidateCache();
            freshness_.set(key.getFreshness());
        }

        if(cache_.assignIfHadKey(key, ptr)){
            return;
        }

        if(om::BLOCKING == blocking)
        {
            ptr = loadItem(key);
            return;
        }

        threadPool_.QueueUpIfKeyNotPresent(key,
                                           &OmThreadedTileCache::handleCacheMissThread,
                                           this);
    }

    void GetDontQueue(ptr_t& ptr, const key_t& key){
        cache_.assignIfHadKey(key, ptr);
    }

    void QueueUp(const key_t& key)
    {
        if(cache_.contains(key)){
            return;
        }

        threadPool_.QueueUpIfKeyNotPresent(key,
                                           &OmThreadedTileCache::handleCacheMissThread,
                                           this);
    }

    void BlockingCreate(ptr_t& ptr, const key_t& key){
        ptr = loadItem(key);
    }

    void Prefetch(const key_t& key, const int depthOffset)
    {
        tilesToPrefetch_.insert(key, depthOffset);

        prefetchItem(tilesToPrefetch_.get_front());
    }

    void Remove(const key_t& key){
        cache_.erase(key);
    }

    void RemoveOldest(const int64_t numBytesToRemove)
    {
        int64_t numBytesRemoved = 0;

        while(numBytesRemoved < numBytesToRemove)
        {
            if(cache_.empty()){
                return;
            }

            const ptr_t ptr = cache_.remove_oldest();

            if(!ptr){
                continue;
            }

            numBytesRemoved += ptr->NumBytes();
        }
    }

    void Clean()
    {
        if(cachesToClean_.empty()){
            return;
        }

        // avoid contention on cacheToClean by swapping in new, empty list
        std::list<om::shared_ptr<OldCache> > oldCaches;
        cachesToClean_.swap(oldCaches);
    }

    void Clear()
    {
        cache_.clear();
        threadPool_.clear();
    }

    void ClearFetchQueue(){
        threadPool_.clear();
    }

    void InvalidateCache()
    {
        ClearFetchQueue();

        // add current cache to list to be cleaned later by OmCacheMan thread
        om::shared_ptr<OldCache> cache(new OldCache());

        cache_.swap(cache->map, cache->list);
        cachesToClean_.push_back(cache);
    }

    int64_t GetCacheSize() const {
        return cache_.size() * bytesPerTile_;
    }

    void CloseDownThreads()
    {
        killingCache_.set(true);
        threadPool_.stop();
        cache_.clear();
    }

private:
    void handleCacheMissThread(const key_t key)
    {
        if(killingCache_.get()){
            return;
        }

        loadItem(key);
    }

    ptr_t loadItem(const key_t& key)
    {
        ptr_t ptr = handleCacheMiss(key);
        cache_.set(key, ptr);
        return ptr;
    }

    void prefetchItem(const key_t& key)
    {
        if(cache_.contains(key)){
            return;
        }

        ptr_t ptr = handleCacheMiss(key);
        cache_.setPrefetch(key, ptr);
    }

    ptr_t handleCacheMiss(const key_t& key)
    {
        OmTile* tile = new OmTile(this, key);
        tile->LoadData();
        return ptr_t(tile);
    }
};

