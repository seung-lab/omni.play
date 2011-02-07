#ifndef OM_THREADED_CACHE_H
#define OM_THREADED_CACHE_H

#include "common/om.hpp"
#include "utility/omLockedPODs.hpp"
#include "system/cache/omLockedCacheObjects.hpp"
#include "utility/omThreadPool.hpp"
#include "system/cache/omCacheBase.h"
#include "zi/omMutex.h"

template <typename T> class LockedList;

/**
 *	Brett Warne - bwarne@mit.edu - 3/12/09
 */

template <typename KEY, typename PTR> class OmHandleCacheMissTask;

template <typename KEY, typename PTR>
class OmThreadedCache : public OmCacheBase {
public:
    OmThreadedCache(const om::CacheGroup group,
                    const std::string& name,
                    const int numThreads,
                    const om::ShouldThrottle,
                    const om::ShouldFifo = om::DONT_FIFO);
    virtual ~OmThreadedCache();

    virtual void Get(PTR&, const KEY&, const om::Blocking);

    void Prefetch(const KEY& key);

    void Remove(const KEY& key);
    int Clean(const bool okToRemoveOldest);
    void Clear();
    void InvalidateCache();

    void UpdateSize(const int64_t delta);

    int GetFetchStackSize() const;
    int64_t GetCacheSize() const;

    virtual PTR HandleCacheMiss(const KEY& key) = 0;

    void closeDownThreads();

private:
    typedef OmHandleCacheMissTask<KEY, PTR> CacheMissHandler;
    typedef boost::shared_ptr<CacheMissHandler> CacheMissHandlerPtr;
    typedef boost::shared_ptr<std::map<KEY,PTR> > OldCachePtr;

    const int numThreads_;

    LockedInt64 curSize_;
    OmThreadPool threadPool_;

    zi::rwmutex mutex_;
    LockedKeySet<KEY> currentlyFetching_;
    LockedCacheMap<KEY, PTR> cache_;
    LockedKeyMultiIndex<KEY> keyAccessList_;
    LockedBool killingCache_;

    boost::shared_ptr<LockedList<OldCachePtr> > cachesToClean_;

    int removeOldest();
    void get(PTR&, const KEY&, const bool);

    const om::ShouldThrottle throttle_;
    const om::ShouldFifo fifo_;

    template <typename T1, typename T2> friend class OmHandleCacheMissTask;
};

#endif
