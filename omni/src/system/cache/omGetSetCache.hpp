#ifndef OM_GET_SET_CACHE_HPP
#define OM_GET_SET_CACHE_HPP

#include "common/om.hpp"
#include "system/cache/omCacheBase.h"
#include "system/cache/omLockedCacheObjects.hpp"
#include "utility/omLockedPODs.hpp"
#include "zi/omMutex.h"

template <typename KEY, typename PTR>
class OmGetSetCache : public OmCacheBase {
private:
    const int64_t entrySize_;

    zi::mutex lock_;
    std::map<KEY, PTR> cache_;
    std::list<KEY> mru_;

    LockedKeyMultiIndex<KEY> full_mru_;

public:
    OmGetSetCache(const std::string& cacheName, const int64_t entrySize)
        : OmCacheBase(cacheName, om::TILE_CACHE)
        , entrySize_(entrySize)
    {}

    virtual ~OmGetSetCache()
    {}

    void Clear()
    {
        zi::guard g(lock_);
        cache_.clear();
        mru_.clear();
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
        cache_[key] = ptr;
    }

    int Clean(const bool)
    {
        std::list<KEY> mru;
        {
            zi::guard g(lock_);
            if(!mru_.empty()){
                mru_.swap(mru);
            }
        }

        FOR_EACH(iter, mru){
            full_mru_.touch(*iter);
        }

        if(full_mru_.empty()){
            return 0;
        }

        const KEY key = full_mru_.remove_oldest();

        int numRemoved;
        {
            zi::guard g(lock_);
            numRemoved = cache_.erase(key);
        }

        return numRemoved;
    }

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
