#pragma once

#include "utility/omLockedPODs.hpp"
#include "zi/omMutex.h"

class OmSegmentColorizerColorCache : public zi::rwmutex {
private:
    LockedUint32 size_;

    struct OmColorWithFreshness
    {
        om::common::Color color;
        uint64_t freshness;
    };

    std::vector<OmColorWithFreshness> cache_;

    struct colorizer_mutex_pool_tag;
    typedef zi::spinlock::pool<colorizer_mutex_pool_tag>::guard spinlock_guard_t;

public:
    OmSegmentColorizerColorCache()
    {
        size_.set(0);
    }

    inline uint32_t Size() const {
        return size_.get();
    }

    inline void Resize(const uint32_t newSize)
    {
        zi::rwmutex::write_guard g(*this);
        size_.set(newSize);
        cache_.resize(newSize);
    }

    inline void Get(const uint32_t val, uint64_t& freshness, om::common::Color& color)
    {
		spinlock_guard_t g(val);
        freshness = cache_[val].freshness;
        color = cache_[val].color;
    }

    inline void Set(const uint32_t val, const uint64_t freshness,
                    const om::common::Color& color)
    {
        spinlock_guard_t g(val);
        cache_[val].freshness = freshness;
        cache_[val].color = color;
    }
};

