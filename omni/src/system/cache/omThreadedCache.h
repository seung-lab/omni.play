#ifndef OM_THREADED_CACHE_H
#define OM_THREADED_CACHE_H

#include "common/omDebug.h"
#include "utility/omLockedObjects.h"
#include "utility/OmThreadPool.hpp"
#include "system/cache/omCacheBase.h"

#include <zi/mutex>

/**
 *	Brett Warne - bwarne@mit.edu - 3/12/09
 */

template < typename KEY, typename PTR  >
class OmThreadedCache : public OmCacheBase {
 public:
	OmThreadedCache(const OmCacheGroupEnum group);
	virtual ~OmThreadedCache();

	//value accessors
	void Get(PTR& p_value,
		 const KEY &key,
		 bool blocking);
	void Remove(const KEY &key);
	void Remove(OmCacheableBase *);
	void RemoveOldest();
	void Flush();

	void UpdateSize(const qint64 delta);

	//get info about the cache
	int GetFetchStackSize();
	qint64 GetCacheSize();

	virtual PTR HandleCacheMiss(const KEY& key) = 0;

	zi::RWMutex mCacheMutex;
	LockedKeySet<KEY> mCurrentlyFetching;
	LockedCacheMap<KEY, PTR> mCache;
	LockedKeyList<KEY> mKeyAccessList;

	void closeDownThreads();

	LockedBool mKillingCache;

private:
	LockedInt64 mCurSize;
	const std::string name_;
	OmThreadPool mThreadPool;
};

#endif
