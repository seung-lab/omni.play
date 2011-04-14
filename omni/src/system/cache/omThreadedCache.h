#ifndef OM_THREADED_CACHE_H
#define OM_THREADED_CACHE_H

#include "common/om.hpp"
#include "system/cache/omCacheBase.h"
#include "system/cache/omCacheManager.h"
#include "system/cache/omLockedCacheObjects.hpp"
#include "utility/omLockedObjects.h"
#include "utility/omLockedPODs.hpp"
#include "utility/omThreadPool.hpp"
#include "zi/omMutex.h"

#include <zi/system.hpp>

/**
 *  Brett Warne - bwarne@mit.edu - 3/12/09
 */

template <typename KEY, typename PTR>
class OmThreadedCache : public OmCacheBase {
private:
    const om::ShouldThrottle throttle_;

    OmThreadPool threadPool_;
    LockedInt64 curSize_;

    LockedCacheMap<KEY, PTR> cache_;
    LockedKeySet<KEY> currentlyFetching_;
    LockedBool killingCache_;

    struct OldCache {
        std::map<KEY,PTR> map;
        KeyMultiIndex<KEY> list;
    };
    LockedList<boost::shared_ptr<OldCache> > cachesToClean_;

    int numThreads()
    {
        int num = zi::system::cpu_count;
        if(num > 8){
            return 8;
        }
        if(num < 2){
            return 2;
        }
        return num;
    }

public:
    OmThreadedCache(const om::CacheGroup group,
                    const std::string& name,
                    const om::ShouldThrottle throttle)
        : OmCacheBase(name, group)
        , throttle_(throttle)
    {
        OmCacheManager::AddCache(group, this);
        threadPool_.start(numThreads());
    }

    virtual ~OmThreadedCache()
    {
        closeDownThreads();
        OmCacheManager::RemoveCache(cacheGroup_, this);
    }

    virtual void Get(PTR& ptr, const KEY& key, const om::Blocking blocking)
    {
        if(cache_.assignIfHadKey(key, ptr)){
            return;
        }

        if(om::BLOCKING == blocking)
        {
            ptr = loadItem(key);
            return;
        }

        if(om::THROTTLE == throttle_ &&
           zi::system::cpu_count == threadPool_.getTaskCount())
        {
            return; // restrict number of tasks to process
        }

        if(currentlyFetching_.insertSinceDidNotHaveKey(key))
        {
            threadPool_.addTaskBack(
                zi::run_fn(
                    zi::bind(&OmThreadedCache<KEY,PTR>::handleCacheMissThread,
                             this, key)));
        }
    }

    virtual PTR HandleCacheMiss(const KEY& key) = 0;

    void BlockingCreate(PTR& ptr, const KEY& key){
        ptr = loadItem(key);
    }

    void Prefetch(const KEY& key){
        prefetchItem(key);
    }

    void Remove(const KEY& key)
    {
        PTR ptr = cache_.erase(key);
        if(!ptr){
            return;
        }
        curSize_.sub(ptr->NumBytes());
    }

    void RemoveOldest(const int64_t numBytesToRemove)
    {
        int64_t numBytesRemoved = 0;

        while(numBytesRemoved < numBytesToRemove)
        {
            if(cache_.empty()){
                return;
            }

            const PTR ptr = cache_.remove_oldest();

            if(!ptr){
                continue;
            }

            const int64_t removedBytes = ptr->NumBytes();
            numBytesRemoved += removedBytes;
            curSize_.sub(removedBytes);
        }
    }

    void Clean()
    {
        if(cachesToClean_.empty()){
            return;
        }

        // avoid contention on cacheToClean by swapping in new, empty list
        std::list<boost::shared_ptr<OldCache> > oldCaches;
        cachesToClean_.swap(oldCaches);
    }

    void Clear()
    {
        cache_.clear();
        currentlyFetching_.clear();
        curSize_.set(0);
    }

    void ClearFetchQueue()
    {
        threadPool_.clear();
        currentlyFetching_.clear();
    }

    void InvalidateCache()
    {
        ClearFetchQueue();

        // add current cache to list to be cleaned later by OmCacheMan thread
        boost::shared_ptr<OldCache> cache(new OldCache());

        cache_.swap(cache->map, cache->list);
        cachesToClean_.push_back(cache);

        curSize_.set(0);
    }

    int64_t GetCacheSize() const {
        return curSize_.get();
    }

    void closeDownThreads()
    {
        killingCache_.set(true);
        threadPool_.stop();
        cache_.clear();
        currentlyFetching_.clear();
    }

private:
    void handleCacheMissThread(const KEY key)
    {
        if(killingCache_.get()){
            return;
        }

        loadItem(key);
    }

    PTR loadItem(const KEY& key)
    {
        PTR ptr = HandleCacheMiss(key);
        const bool wasInserted = cache_.set(key, ptr);
        if(wasInserted){
            curSize_.add(ptr->NumBytes());
        }
        return ptr;
    }

    void prefetchItem(const KEY& key)
    {
        if(cache_.contains(key)){
            return;
        }

        PTR ptr = HandleCacheMiss(key);
        const bool wasInserted = cache_.setPrefetch(key, ptr);
        if(wasInserted){
            curSize_.add(ptr->NumBytes());
        }
    }
};

#endif
