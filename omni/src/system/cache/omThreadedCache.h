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

template <typename KEY, typename PTR> class HandleCacheMissThreaded;

template <typename KEY, typename PTR>
class OmThreadedCache : public OmCacheBase {
public:
	OmThreadedCache(const OmCacheGroupEnum group,
			const std::string& name);
	virtual ~OmThreadedCache();

	//value accessors
	virtual void Get(PTR&, const KEY&, const bool);
	virtual void Get(PTR&, const KEY&, const OM::BlockingRead);
	void Remove(const KEY &key);
	int Clean();
	void Flush();
	void Clear(); //TODO: remove me
	void InvalidateCache();
	const std::string& GetName(){ return name_; }

	void UpdateSize(const qint64 delta);

	//get info about the cache
	int GetFetchStackSize();
	qint64 GetCacheSize();

	virtual PTR HandleCacheMiss(const KEY& key) = 0;

	void closeDownThreads();

private:
	typedef HandleCacheMissThreaded<KEY, PTR> CacheMissHandler;
	typedef boost::shared_ptr<CacheMissHandler> CacheMissHandlerPtr;
	typedef boost::shared_ptr<std::map<KEY,PTR> > OldCachePtr;

	LockedInt64 mCurSize;
	const std::string name_;
	OmThreadPool mThreadPool;

	zi::mutex mutex_;
	LockedKeySet<KEY> mCurrentlyFetching;
	LockedCacheMap<KEY, PTR> mCache;
	LockedKeyList<KEY> mKeyAccessList;
	LockedBool mKillingCache;

	int RemoveOldest();
	boost::shared_ptr<LockedList<OldCachePtr> > cachesToClean_;

	template <typename T1, typename T2> friend class HandleCacheMissThreaded;
};

#endif
