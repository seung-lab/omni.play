#include "system/cache/omCacheManager.h"
#include "system/cache/omHandleCacheMissTask.hpp"
#include "system/cache/omThreadedCache.h"
#include "utility/omLockedObjects.h"
#include "utility/stringHelpers.h"

template <typename KEY, typename PTR>
OmThreadedCache<KEY,PTR>::OmThreadedCache(const OmCacheGroupEnum group,
										  const std::string& name,
										  const int numThreads,
										  const om::ShouldThrottle throttle,
										  const om::ShouldFifo fifo)
	: OmCacheBase(group)
	, name_(name)
	, numThreads_(numThreads)
	, cachesToClean_(boost::make_shared<LockedList<OldCachePtr> >())
	, throttle_(throttle)
	, fifo_(fifo)
{
	OmCacheManager::AddCache(group, this);
	threadPool_.start(numThreads_);
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
	killingCache_.set(true);
	threadPool_.stop();
	cache_.clear();
	keyAccessList_.clear();
	currentlyFetching_.clear();
}

template <typename KEY, typename PTR>
void OmThreadedCache<KEY,PTR>::UpdateSize(const int64_t delta)
{
	curSize_.add(delta);
	//	assert( curSize_.get() >= 0 );
}

/**
 *	Get value from cache associated with given key.
 *	Specify if Get should block calling thread.
 */
template <typename KEY, typename PTR>
void OmThreadedCache<KEY,PTR>::get(PTR &p_value,
								   const KEY &key,
								   const bool blocking)
{
	if( cache_.setIfHadKey(key, p_value) ) {
		// done!
	} else if(blocking || !numThreads_){
		p_value = HandleCacheMiss(key);
		{
			zi::rwmutex::write_guard g(mutex_);
			cache_.set(key, p_value);
			keyAccessList_.touch(key);
		}
	} else {

		if(om::THROTTLE == throttle_){
			if(threadPool_.getTaskCount() ==
			   threadPool_.getMaxSimultaneousTaskCount()) {
				return; // restrict number of tasks to process
			}
		}

		if(currentlyFetching_.insertSinceDidNotHaveKey(key) ){
			CacheMissHandlerPtr task =
				boost::make_shared<CacheMissHandler>(this, key);
			if(om::FIFO == fifo_){
				threadPool_.addTaskBack(task);
			} else {
				threadPool_.addTaskFront(task);
			}
		}
	}
}

template <typename KEY, typename PTR>
void OmThreadedCache<KEY,PTR>::Get(PTR& p_value,
								   const KEY &key,
								   const om::Blocking blocking)
{
	if(om::BLOCKING == blocking){
		get(p_value, key, true);
	} else {
		get(p_value, key, false);
	}
}

template <typename KEY, typename PTR>
void OmThreadedCache<KEY,PTR>::Prefetch(const KEY &key)
{
	if(cache_.contains(key)){
		return;
	}

	PTR val = HandleCacheMiss(key);
	cache_.set(key, val);
	keyAccessList_.touchPrefetch(key);
}

template <typename KEY, typename PTR>
void OmThreadedCache<KEY,PTR>::Remove(const KEY &key)
{
	cache_.erase(key);
}

template <typename KEY, typename PTR>
int OmThreadedCache<KEY,PTR>::Clean(const bool okToRemoveOldest)
{
	if(cachesToClean_->empty()){
		if(okToRemoveOldest){
			return removeOldest();
		}
		return 0;
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
int OmThreadedCache<KEY,PTR>::removeOldest()
{
	zi::rwmutex::write_guard g(mutex_);

	if(cache_.empty() || keyAccessList_.empty()){
		return 0;
	}

	const KEY key = keyAccessList_.remove_oldest();
	const PTR val = cache_.get(key);
	if(!val){
		return 0;
	}

	val->Flush();
	curSize_.sub(val->NumBytes());
	cache_.erase(key);

	return 1;
}

template <typename KEY, typename PTR>
int OmThreadedCache<KEY,PTR>::GetFetchStackSize()
{
	return threadPool_.getTaskCount();
}

template <typename KEY, typename PTR>
int64_t OmThreadedCache<KEY,PTR>::GetCacheSize()
{
	/*
	  if(RAM_CACHE_GROUP == mCacheGroup ){
	  std::cout << "current cache (" << getGroupName() << ") size is :"
	  << StringHelpers::commaDeliminateNumber(curSize_.get()).toStdString()
	  << " bytes\n";
	  }
	*/
	return curSize_.get();
}

template <typename KEY, typename PTR>
void OmThreadedCache<KEY,PTR>::Clear()
{
	zi::rwmutex::write_guard g(mutex_);
	cache_.clear();
	curSize_.set(0);
}

template <typename KEY, typename PTR>
void OmThreadedCache<KEY,PTR>::InvalidateCache()
{
	zi::rwmutex::write_guard g(mutex_);

	threadPool_.clear();

	// add current cache to list to be cleaned later by OmCacheMan thread
	OldCachePtr cache(new std::map<KEY,PTR>());
	cache_.swap(*cache);
	cachesToClean_->push_back(cache);

	keyAccessList_.clear();
	currentlyFetching_.clear();
	curSize_.set(0);
}
