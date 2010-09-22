#include "utility/omLockedObjects.h"
#include "omThreadedCache.h"
#include "omHandleCacheMissThreaded.h"
#include "utility/stringHelpers.h"
#include "system/cache/omCacheManager.h"
#include <boost/make_shared.hpp>

static const int MAX_THREADS = 3;

template <typename KEY, typename PTR>
OmThreadedCache<KEY,PTR>::OmThreadedCache(const OmCacheGroupEnum group,
					  const std::string& name)
	: OmCacheBase(group)
	, name_(name)
	, cachesToClean_(boost::make_shared<LockedList<OldCachePtr> >())
{
	OmCacheManager::AddCache(group, this);
	mThreadPool.start(MAX_THREADS);
}

template <typename KEY, typename PTR>
OmThreadedCache<KEY,PTR>::~OmThreadedCache()
{
	closeDownThreads();
	OmCacheManager::RemoveCache(mCacheGroup, this);
}

template <typename KEY, typename PTR>
void OmThreadedCache<KEY,PTR>::closeDownThreads()
{
	mKillingCache.set(true);
	mThreadPool.stop();
	mCache.flush();
	mCache.clear();
	mKeyAccessList.clear();
	mCurrentlyFetching.clear();
}

template <typename KEY, typename PTR>
void OmThreadedCache<KEY,PTR>::UpdateSize(const qint64 delta)
{
	mCurSize.add(delta);
	//	assert( mCurSize.get() >= 0 );
}

/**
 *	Get value from cache associated with given key.
 *	Specify if Get should block calling thread.
 */
template <typename KEY, typename PTR>
void OmThreadedCache<KEY,PTR>::Get(PTR &p_value,
				   const KEY &key,
				   const bool blocking)
{
	if( mCache.setIfHadKey(key, p_value) ) {
		// done!
	} else if(blocking) {
		p_value = HandleCacheMiss(key);
		{
			zi::Guard g(mCacheMutex);
			mCache.set(key, p_value);
			mKeyAccessList.touch(key);
		}
	} else {
		if(mThreadPool.getTaskCount() ==
		   mThreadPool.getMaxSimultaneousTaskCount()) {
			return; // restrict number of tasks to process
		}

		if(mCurrentlyFetching.insertSinceDidNotHaveKey(key) ){
			CacheMissHandlerPtr task =
				boost::make_shared<CacheMissHandler>(this, key);
			mThreadPool.addTaskFront(task);
		}
	}
}

template <typename KEY, typename PTR>
void OmThreadedCache<KEY,PTR>::Get(PTR& p_value,
				   const KEY &key,
				   const OM::BlockingRead blocking)
{
	if(OM::BLOCKING == blocking){
		Get(p_value, key, true);
	} else {
		Get(p_value, key, false);
	}
}

template <typename KEY, typename PTR>
void OmThreadedCache<KEY,PTR>::Remove(const KEY &key)
{
	mCache.erase(key);
}

template <typename KEY, typename PTR>
int OmThreadedCache<KEY,PTR>::Clean()
{
	if(cachesToClean_->empty()){
		return RemoveOldest();
	}

	// avoid contention on cacheToClean by swapping in new, empty list
	std::list<OldCachePtr> oldCaches;
	cachesToClean_->swap(oldCaches);

	int numItems = 0;
	FOR_EACH(iter, oldCaches){
		OldCachePtr cache = *iter;
		numItems += cache->size();
		cache->clear();
	}
	oldCaches.clear();

	return numItems;
}

template <typename KEY, typename PTR>
int OmThreadedCache<KEY,PTR>::RemoveOldest()
{
	zi::Guard g(mCacheMutex);

	if(mCache.empty() || mKeyAccessList.empty()){
		return 0;
	}

	const KEY key = mKeyAccessList.remove_back();
	mCache.erase(key);
	return 1;
}

template <typename KEY, typename PTR>
int OmThreadedCache<KEY,PTR>::GetFetchStackSize()
{
	return mThreadPool.getTaskCount();
}

template <typename KEY, typename PTR>
qint64 OmThreadedCache<KEY,PTR>::GetCacheSize()
{
	/*
	if(RAM_CACHE_GROUP == mCacheGroup ){
	  std::cout << "current cache (" << getGroupName() << ") size is :"
	  << StringHelpers::commaDeliminateNumber(mCurSize.get()).toStdString()
	  << " bytes\n";
	}
	*/
        return mCurSize.get();
}

template <typename KEY, typename PTR>
void OmThreadedCache<KEY,PTR>::Flush()
{
	mCache.flush();
}

template <typename KEY, typename PTR>
void OmThreadedCache<KEY,PTR>::Clear()
{
	mCache.clear();
}

template <typename KEY, typename PTR>
void OmThreadedCache<KEY,PTR>::InvalidateCache()
{
	zi::Guard g(mCacheMutex);

	mThreadPool.clear();

	// add current cache to list to be cleaned later by OmCacheMan thread
	OldCachePtr cache(new std::map<KEY,PTR>());
	mCache.swap(*cache);
	cachesToClean_->push_back(cache);

	mKeyAccessList.clear();
	mCurrentlyFetching.clear();
}
