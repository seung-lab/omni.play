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
 *  Brett Warne - bwarne@mit.edu - 3/12/09
 */

template <typename KEY, typename PTR> class OmHandleCacheMissTask;

template <typename KEY, typename PTR>
class OmThreadedCache : public OmCacheBase {
public:
    OmThreadedCache(const om::CacheGroup group,
                    const std::string& name,
                    const int numThreads,
                    const om::ShouldThrottle,
                    const om::ShouldFifo,
                    const int64_t entrySize);

    virtual ~OmThreadedCache();

    virtual void Get(PTR&, const KEY&, const om::Blocking);
    virtual PTR HandleCacheMiss(const KEY& key) = 0;

    void BlockingCreate(PTR&, const KEY&);
    void Prefetch(const KEY& key);

    size_t Remove(const KEY& key);
    void RemoveOldest(const int64_t numBytesToRemove);
    void Clean();
    void Clear();
    void InvalidateCache();

    int64_t GetCacheSize() const;

    void closeDownThreads();

private:
    typedef OmHandleCacheMissTask<KEY, PTR> CacheMissHandler;
    typedef boost::shared_ptr<CacheMissHandler> CacheMissHandlerPtr;
    typedef boost::shared_ptr<std::map<KEY,PTR> > OldCachePtr;

    const int numThreads_;
    const om::ShouldThrottle throttle_;
    const om::ShouldFifo fifo_;
    const int64_t entrySize_;

    LockedInt64 curSize_;
    OmThreadPool threadPool_;

    zi::rwmutex mutex_;
    LockedKeySet<KEY> currentlyFetching_;
    LockedCacheMap<KEY, PTR> cache_;
    LockedKeyMultiIndex<KEY> keyAccessList_;
    LockedBool killingCache_;

    boost::scoped_ptr<LockedList<OldCachePtr> > cachesToClean_;

    void get(PTR&, const KEY&, const bool);
    void updateSize(const int64_t delta);
    int64_t doRemoveOldest();

    template <typename T1, typename T2> friend class OmHandleCacheMissTask;
};

#endif
