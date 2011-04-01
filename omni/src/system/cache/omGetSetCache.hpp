#ifndef OM_GET_SET_CACHE_HPP
#define OM_GET_SET_CACHE_HPP

#include "common/om.hpp"
#include "system/cache/omCacheBase.h"
#include "system/cache/omCacheManager.h"
#include "system/cache/omLockedCacheObjects.hpp"
#include "zi/omMutex.h"

/**
 * simple locked-protected cache of fixed-sized objects
 *
 * will be memory-limited by OmCacheManager
 *
 * Michael Purcaro 02/2011
 **/

template <typename KEY, typename PTR>
class OmGetSetCache : public OmCacheBase {
private:
    const om::CacheGroup cacheGroup_;
    const int64_t entrySize_;

    zi::spinlock lock_;

    std::map<KEY, PTR> cache_;

    std::list<KEY> mru_; // most recent keys at end of list

    LockedKeyMultiIndex<KEY> full_mru_;

public:
    OmGetSetCache(const om::CacheGroup cacheGroup, const std::string& cacheName,
                  const int64_t entrySize)
        : OmCacheBase(cacheName, cacheGroup)
        , cacheGroup_(cacheGroup)
        , entrySize_(entrySize)
    {
        OmCacheManager::AddCache(cacheGroup_, this);
    }

    virtual ~OmGetSetCache(){
        OmCacheManager::RemoveCache(cacheGroup_, this);
    }

    void Clear()
    {
        {
            zi::guard g(lock_);
            cache_.clear();
            mru_.clear();
        }
        full_mru_.clear();
    }

    inline PTR Get(const KEY& key)
    {
        zi::guard g(lock_);
        mru_.push_back(key);
        return cache_[key];
    }

    inline void Set(const KEY& key, const PTR& ptr)
    {
        zi::guard g(lock_);
        mru_.push_back(key);
        cache_[key] = ptr;
    }

    void Clean()
    {
        std::list<KEY> mru;
        {
            zi::guard g(lock_);
            mru_.swap(mru);
        }

        // merge in keys got/set since last clean
        full_mru_.touch(mru);
    }

    void RemoveOldest(const int64_t numBytesToRemove)
    {
        if(full_mru_.empty()){
            return;
        }

        const int numToRemove = numBytesToRemove / entrySize_;

        for(int i = 0; i < numToRemove; ++i){
            if(full_mru_.empty()){
                return;
            }
            const KEY key = full_mru_.remove_oldest();
            {
                zi::guard g(lock_);
                cache_.erase(key);
            }
        }
    }

    // cache of fixed-object sizes, so compute size
    int64_t GetCacheSize() const
    {
        int64_t numSlices;
        {
            zi::guard g(lock_);
            numSlices = cache_.size();
        }
        return numSlices * entrySize_;
    }
};

#endif
