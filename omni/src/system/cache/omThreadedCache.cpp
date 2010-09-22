#include "omThreadedCache.h"
#include "omHandleCacheMissThreaded.h"
#include "utility/stringHelpers.h"
#include "system/cache/omCacheManager.h"

static const int MAX_THREADS = 3;

template < typename KEY, typename PTR  >
OmThreadedCache<KEY,PTR>::OmThreadedCache(const OmCacheGroupEnum group)
	: OmCacheBase(group)
{
	OmCacheManager::AddCache(group, this);
	mThreadPool.start(MAX_THREADS);
}

template < typename KEY, typename PTR  >
OmThreadedCache<KEY,PTR>::~OmThreadedCache()
{
	closeDownThreads();
	OmCacheManager::RemoveCache(mCacheGroup, this);
}

template < typename KEY, typename PTR  >
void OmThreadedCache<KEY,PTR>::closeDownThreads()
{
	mKillingCache.set(true);
	mThreadPool.stop();
	mCache.flush();
	mCache.clear();
	mKeyAccessList.clear();
	mCurrentlyFetching.clear();
}

template < typename KEY, typename PTR  >
void OmThreadedCache<KEY,PTR>::UpdateSize(const qint64 delta)
{
	mCurSize.add(delta);
	//	assert( mCurSize.get() >= 0 );
}

/**
 *	Get value from cache associated with given key.
 *	Specify if Get should block calling thread.
 */
template < typename KEY, typename PTR  >
void OmThreadedCache<KEY,PTR>::Get(PTR &p_value,
				   const KEY &key,
				   const bool blocking)
{
	if( mCache.setIfHadKey(key, p_value) ) {
		// done!
	} else if(blocking) {
		p_value = HandleCacheMiss(key);
		{
			zi::WriteGuard g(mCacheMutex);
			mCache.set(key, p_value);
			mKeyAccessList.touch(key);
		}
	} else {
		if(mThreadPool.getTaskCount() ==
		   mThreadPool.getMaxSimultaneousTaskCount()) {
			return; // restrict number of tasks to process
		}

		if(mCurrentlyFetching.insertSinceDidNotHaveKey(key) ){
			boost::shared_ptr<HandleCacheMissThreaded<OmThreadedCache<KEY, PTR>, KEY, PTR> >
				task(new HandleCacheMissThreaded<OmThreadedCache<KEY, PTR>, KEY, PTR>(this, key));
			mThreadPool.addTaskFront(task);
		}
	}
}

template < typename KEY, typename PTR  >
void OmThreadedCache<KEY,PTR>::Remove(const KEY &key)
{
	mCache.erase(key);
}

template < typename KEY, typename PTR  >
void OmThreadedCache<KEY,PTR>::RemoveOldest()
{
	zi::WriteGuard g(mCacheMutex);

	if(mCache.empty() || mKeyAccessList.empty()){
		return;
	}
	const KEY key = mKeyAccessList.remove_back();
	mCache.erase(key);
}

template < typename KEY, typename PTR  >
int OmThreadedCache<KEY,PTR>::GetFetchStackSize()
{
	return mThreadPool.getTaskCount();
}

template < typename KEY, typename PTR  >
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

template < typename KEY, typename PTR  >
void OmThreadedCache<KEY,PTR>::Flush()
{
	mCache.flush();
}

template < typename KEY, typename PTR  >
void OmThreadedCache<KEY,PTR>::Clear()
{
	mCache.clear();
}
