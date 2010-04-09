#ifndef OM_THREADED_CACHE_H
#define OM_THREADED_CACHE_H


#include "omCacheBase.h"

#include "utility/stackSet.h"
#include "common/omException.h"
#include "common/omThreads.h"
#include "common/omDebug.h"
#include <boost/shared_ptr.hpp>

using boost::shared_ptr;

#include <list>
using std::list;

#include <time.h>


#define OM_DEFAULT_FETCH_UPDATE_CLEARS_FETCH_STACK 0
#define OM_DEFAULT_FETCH_UPDATE_INTERVAL_SECONDS 0.5f

#define OM_THREADED_CACH_DEBUG 0


//fetch thread caller prototype
template < typename T,  typename U  > void* start_cache_fetch_thread(void*);

/**
 *	A templated non-blocking threaded cache system that retains most recently used objects.
 *
 *	This modifies the GenericCache by having a second thread specifically tasked to fetch
 *	values from disk.  Keys are built up by the main thread in a double-ended queue which
 *	allows the specific fetching prioritization can be specified as either FIFO or LIFO.
 *
 *	Care must be taken so that calls to parent methods don't contain overridden virtual 
 *	methods that would result in double locking in the child.
 *
 *	Brett Warne - bwarne@mit.edu - 3/12/09
 */

template < typename T,  typename U  >
class OmThreadedCache : public OmCacheBase {
		
public:
	OmThreadedCache(OmCacheGroup group, bool initFetchThread = false);
	virtual ~OmThreadedCache();
	
	
	//value accessors
	void Get(shared_ptr<U> &p_value,const T &key, bool blocking = false, bool exist = false);
	void Add(const T &key, U *value);
	void Remove(const T &key);
	bool RemoveOldest();
	bool Contains(const T &key);
	void Call(void (U::*fxn)() , bool lock = true);
	void Clear();
	
	//fetch thread
	bool IsFetchStackEmpty();
	void FetchLoop();
	bool FetchUpdateCheck();
	virtual void HandleFetchUpdate() { };
	virtual bool InitializeFetchThread();
	virtual void KillFetchLoop();
	
	//fetch properties
	void SetFetchUpdateInterval(float);
	float GetFetchUpdateInterval();
	void SetFetchUpdateClearsFetchStack(bool);
	bool GetFetchUpdateClearsFetchStack();
	
	/**Name function for debugging */
	void SetCacheName(const char* name);
	void GetCacheName(char* name);
	
protected:
	virtual U* HandleCacheMiss(const T &key) = 0;
	
	
private:
	//key, shared pointer value cache
	map< T, shared_ptr< U > > mCachedValuesMap;
	//least recently accessed at head of list
	list< T > mKeyAccessList;
	
	
	StackSet< T > mFetchStack;	//stack of keys to be fetched
	set< T > mCurrentlyFetching;		//set of keys currently being fetched
	
	bool mFetchThreadAlive;
	bool mInitializeFetchThread;
	bool mKillingFetchThread;		//note that fetch thread needs to die
	bool mFetchThreadQueuing;	//note if fetch thread is currently looping through queue
								//Clean() should be ignored while queuing so queue doesn't wipe
								//away it's own work
	bool mFetchThreadCalledClean;
	

	//mutex for cache
	pthread_mutex_t mCacheMutex;

	//fetch thread
	pthread_t mFetchThread;
	pthread_mutex_t mFetchThreadMutex;
	pthread_cond_t mFetchThreadCv;
	
	//fetch thread update
	time_t mLastUpdateTime;
	
	//fetch update prefs
	float mFetchUpdateInterval;
	bool mFetchUpdateClearsStack;

	/** name/descriptor of Cache for debugging */
	char mCacheName[40];
	void * threadSelf;
};





/////////////////////////////////
///////		 OmThreadedCache



/*
 *	Constructor initializes and starts the fetch thread.
 */
template < typename T,  typename U  >
OmThreadedCache<T,U>::OmThreadedCache(OmCacheGroup group, bool initFetch)
: OmCacheBase(group) { 
	

	//fetch prefs
	mFetchUpdateInterval = OM_DEFAULT_FETCH_UPDATE_INTERVAL_SECONDS;
	mFetchUpdateClearsStack = OM_DEFAULT_FETCH_UPDATE_CLEARS_FETCH_STACK;
	mInitializeFetchThread = initFetch;
	
	//init state variables
	mLastUpdateTime = 0;
	mFetchThreadAlive = false;
	mKillingFetchThread = false;
	
	//cache mutex
	pthread_mutex_init(&mCacheMutex, NULL);

	//fetch thread
	pthread_mutex_init(&mFetchThreadMutex, NULL);
	pthread_cond_init(&mFetchThreadCv, NULL);
	
	//set name to "blank"
	SetCacheName("blank");

	//create thread
	
	pthread_create(&mFetchThread, NULL, start_cache_fetch_thread<T,U>, (void *)this);
     	

	//wait for fetch thread
	while(!mFetchThreadAlive) { }
}




/**
 *	Destructor ensures fetch thread is dead before destructing cache.
 */
template < typename T,  typename U  >
OmThreadedCache<T,U>::~OmThreadedCache() {

	debug("destroy","%s Cache %p being destroyed . . .\n",mCacheName,threadSelf);
	
	//send signal to kill fetch thread
	mKillingFetchThread = true;
	pthread_cond_signal(&mFetchThreadCv);

	//spin until done killing
	while(mFetchThreadAlive) { }
}









/////////////////////////////////
///////		 Value Accessors


/*
 *	Get value from cache associated with given key.
 *	Specify if Get should block calling thread.
 *
 *	note: blocking chace does not switch threads
 */
template < typename T,  typename U  > 
void 
  OmThreadedCache<T,U>::Get(shared_ptr<U> &p_value,const T &key, bool blocking, bool exists) {
	
	//std::cerr << "OmThreadedCache::" << __FUNCTION__ << endl;
	//return pointer

  	OmCacheManager::Instance()->CleanCacheGroup(mCacheGroup);
	
	//lock cache
	pthread_mutex_lock(&mCacheMutex);
	
	//check cache
	if(mCachedValuesMap.count(key)) {
		//if in cache, return get value pointer
		p_value = mCachedValuesMap[key];
		
		//debug("FIXME", << "value was in cache: " << p_value << endl;
	} else if(blocking) {
		//unlock cache
		pthread_mutex_unlock(&mCacheMutex);

		//if blocking cache
		//debug("FIXME", << "OmThreadedCache<T,U>::Get(): blocking fetch" << endl;
		shared_ptr<U> tmp = shared_ptr<U>(HandleCacheMiss(key));

		//lock cache
		pthread_mutex_lock(&mCacheMutex);

		//add to cache map
		mCachedValuesMap[key] = tmp;
		//add to access list
		mKeyAccessList.push_front(key);
		
		//get pointer
		p_value = mCachedValuesMap[key];
		
	} else {
		//not in cache so return null pointer
		p_value = shared_ptr<U>();
		
		//if not already in stack and not currently being fetched
		if( (0 == mFetchStack.count(key)) && (0 == mCurrentlyFetching.count(key)) ) {
			// debug("FIXME", << "OmThreadedCache<T,U>::Get(): threaded fetch" << endl;
			//push to top of stack
			if (mFetchStack.size () > 200 ) {
				mFetchStack.clear ();
			}
			mFetchStack.push(key);
			//signal fetch thread
			pthread_cond_signal(&mFetchThreadCv);
		}	
		
	}

	//unlock cache
	pthread_mutex_unlock(&mCacheMutex);
	
	//return cached value or NULL if miss
	return;
}





/**
 *	Add key value pair to cache.
 *
 *	Although Add does not increase the size of the cache, it does
 *	destruct a value.  This destructor may call UpdateSize() which
 *	may try to clean the cache.  This means we need to destruct the
 *	value outside of the cache lock.
 */
template < typename T,  typename U  > 
void
OmThreadedCache<T,U>::Add(const T &key, U *value) {

	//if there was an older vaue with same key
	shared_ptr<U> old_value;
	
	//lock cache and add
	pthread_mutex_lock(&mCacheMutex);
	
	//if already in cache
	if(0 != mCachedValuesMap.count(key)) {
		
		//check if already in use
		if(mCachedValuesMap[key].use_count() > 1)
			assert(false && "Specified cache object already in use.");
		
		//get ref to old value
		old_value = mCachedValuesMap[key];
		
		//then remove from cache
		mCachedValuesMap.erase(key);
		mKeyAccessList.remove(key);
	}
	
	//add to cache map
	mCachedValuesMap[key] = shared_ptr<U>(value);
	//add to access list
	mKeyAccessList.push_front(key);
	
	old_value.reset();

	//unlock
	pthread_mutex_unlock(&mCacheMutex);
	
}



/*
 *	Removes value from the cache.
 *
 *	throws: modification exception if key is in use
 */
template < typename T,  typename U  > 
void
OmThreadedCache<T,U>::Remove(const T &key) {
	//std::cerr << "OmThreadedCache::" << __FUNCTION__ << endl;
	
	//so as to destroy value outside of mutex
	shared_ptr<U> old_value;
	
	//lock cache and add
	pthread_mutex_lock(&mCacheMutex);
	
	//check if already in use
	if(mCachedValuesMap[key].use_count() > 1) {
		//assert(false && "Cache object cannot be removed since it is in use.");
	}

	
	//get ref to old value
	old_value = mCachedValuesMap[key];
	
	//then remove from cache
	mCachedValuesMap.erase(key);
	mKeyAccessList.remove(key);
	
	old_value.reset();

	//unlock
	pthread_mutex_unlock(&mCacheMutex);
	
}



/*
 *	Removes oldest value from the cache.
 *
 *	throws: if in use, the key is moved to head of access list
 */
template < typename T,  typename U  > 
bool
OmThreadedCache<T,U>::RemoveOldest() {
 	bool retTrue = false;	
	debug("cache"," removing oldest...");

	//so as to destroy value outside of mutex
	shared_ptr<U> old_value = shared_ptr<U> ();
	
	//lock cache and add
	pthread_mutex_lock(&mCacheMutex);
	
	//return if no values in cache
	if(!mCachedValuesMap.size()) {
		//unlock
		pthread_mutex_unlock(&mCacheMutex);
		return false;
	}
	
	//assert(mKeyAccessList.size());
	
	//debug("FIXME", << "mKeyAccessList.size(): " << mKeyAccessList.size() << endl;
	if (mKeyAccessList.size() > 20) {
		//get oldest key
		T& r_oldest_key = mKeyAccessList.back();
	
		//check if already in use
		if(mCachedValuesMap[r_oldest_key].use_count() > 1) {
			//in use, so move key to front of access list so that
			//we try to remove other keys first
		
			//pop oldest key
			mKeyAccessList.pop_back();
			//place oldest key at front
			mKeyAccessList.push_front(r_oldest_key);
		
			//unlock
			pthread_mutex_unlock(&mCacheMutex);
			return false;
		}
	
		//get ref to old value
		old_value = mCachedValuesMap[r_oldest_key];

		//debug("FIXME", << "ref count" << (mCachedValuesMap[r_oldest_key].use_count()) << endl;
		//debug("FIXME", << "OmThreadedCache<T,U>::RemoveOldest(): access list size: " << mKeyAccessList.size() << endl;
		//debug("FIXME", << "OmThreadedCache<T,U>::RemoveOldest(): map size: " << mCachedValuesMap.size() << endl;
		assert(r_oldest_key == mKeyAccessList.back());
	
		//then remove oldest from cache
		mCachedValuesMap.erase(r_oldest_key);

		//remove oldest from access list
		mKeyAccessList.pop_back();
		retTrue = true;
	}

	
	if (old_value) 
		old_value.reset();


	//unlock
	pthread_mutex_unlock(&mCacheMutex);
	
	return retTrue;
}




/*
 *	Checks if value is in cache.
 */
template < typename T,  typename U  > 
bool
OmThreadedCache<T,U>::Contains(const T &key) {
	bool status;
	
	pthread_mutex_lock(&mCacheMutex);
	status = mCachedValuesMap.count(key);
	pthread_mutex_unlock(&mCacheMutex);
	
	return status;
}



/*
 *	Call function of void U::*() on all values U in the cache.
 *
 *	The function it calls cannot modify the cache.
 */
template < typename T,  typename U > 
void
OmThreadedCache<T,U>::Call( void (U::*fxn)() , bool lock) {
	
	//lock

	pthread_mutex_lock(&mCacheMutex);
	
	//call on all values of cache map
	typename map< T, shared_ptr< U > >::iterator it;
	for( it=mCachedValuesMap.begin(); it != mCachedValuesMap.end(); it++ ) {

		//mDelayDelta = true;
		(*(it->second).*fxn)();
		//mDelayDelta = false;

		//(*(it->second)).UpdateSize(0);
	}
	
	pthread_mutex_unlock(&mCacheMutex);
}



/*
 *	Clear all elements from the cache
 */
template < typename T,  typename U > 
void
OmThreadedCache<T,U>::Clear() {
	
	//FIXME: this is all broken! must be, at least, mutex protected...
	//while map contains values
	//while(mCachedValuesMap.size()) 
	//	RemoveOldest();
}







/////////////////////////////////
///////		 Fetch Properties


template < typename T,  typename U  >
void
OmThreadedCache<T,U>::SetFetchUpdateInterval(float interval) {
	mFetchUpdateInterval = interval;
}

template < typename T,  typename U  >
float
OmThreadedCache<T,U>::GetFetchUpdateInterval() {
	return mFetchUpdateInterval; 
}

template < typename T,  typename U  >
void
OmThreadedCache<T,U>::SetFetchUpdateClearsFetchStack(bool state) {
	mFetchUpdateClearsStack = state;
}

template < typename T,  typename U  >
bool
OmThreadedCache<T,U>::GetFetchUpdateClearsFetchStack() {
	return mFetchUpdateClearsStack;
}











/////////////////////////////////
///////		 Fetching

template < typename T,  typename U  >
bool
OmThreadedCache<T,U>::IsFetchStackEmpty() {
	bool empty;
	pthread_mutex_lock(&mCacheMutex);
	empty = mFetchStack.empty();
	pthread_mutex_unlock(&mCacheMutex);
	return empty;
}



/*
 *	This loop is only performed by the Fetch Thread.  It waits on the
 *	conditional variable triggered by the main thread when a new element
 *	is added to the deque, and loops until the deque is empty.
 */
template < typename T,  typename U  >
void
OmThreadedCache<T,U>::FetchLoop() {

	//set some variables
	threadSelf = (void*)pthread_self();
	bool checked = false;


	//alive and well
	mFetchThreadAlive = true;
	
	//if calling init fetch thread method, wait until successfully called overriden function in child
	while(!mKillingFetchThread && mInitializeFetchThread && !InitializeFetchThread()) { }
	
	//initially lock mutex
	pthread_mutex_lock(&mFetchThreadMutex);
	
	//forever loop
	while(true) {

		//check if name has been set  . . . if so print it
		while(!checked){
			pthread_mutex_lock(&mCacheMutex);
			if (strcmp(mCacheName,"blank")){
				debug("thread","%s cache thread # %p has started fetch loop\n",mCacheName,threadSelf);
				checked=true;
			}
			pthread_mutex_unlock(&mCacheMutex);
		}
		
		//if destructing, kill thread
		if(mKillingFetchThread) break;
		
		//free mutex and wait for signal
		// debug("FIXME", << "OmThreadedCache<T,U>::FetchLoop(): sleeping " << endl;
		pthread_cond_wait(&mFetchThreadCv, &mFetchThreadMutex);
		// debug("FIXME", << "OmThreadedCache<T,U>::FetchLoop(): awake " << endl;
		//lock mutex and continue
		
		//if destructing, kill thread
		if(mKillingFetchThread) break;
		
		//loop until deque is empty
		mFetchThreadCalledClean = false;
		mFetchThreadQueuing = true;
		
		while (!IsFetchStackEmpty ()) {


			//if destructing, break loop
			if(mKillingFetchThread) break;
			
			//get and pop next in queue
			//debug("FIXME", << "OmThreadedCache<T,U>::FetchLoop(): lock mutex" << endl;
			pthread_mutex_lock(&mCacheMutex);
			
			//fetch stack still non-empty since fetch update could not yet have cleared it
			T fetch_key = mFetchStack.top();
			mFetchStack.pop();
			mCurrentlyFetching.insert(fetch_key);
			
			pthread_mutex_unlock(&mCacheMutex);
			// debug("FIXME", << "OmThreadedCache<T,U>::FetchLoop(): unlock mutex" << endl;
			
			//init returned pointer from cache miss call
			U* fetch_value = NULL;
			
			//any exception causes cache to skip
			fetch_value = HandleCacheMiss(fetch_key);
			
			pthread_mutex_lock(&mCacheMutex);
			//add to cache map
			mCachedValuesMap[fetch_key] = shared_ptr<U>(fetch_value);
			//add to access list
			mKeyAccessList.push_front(fetch_key);
			pthread_mutex_unlock(&mCacheMutex);
			
			
			//key has been fetched, so remove from currently fetching
			mCurrentlyFetching.clear();
			
			//send update if needed
			if(FetchUpdateCheck()) HandleFetchUpdate();
		} 
	
		
		//out of while loop
		mFetchThreadQueuing = false;
		//deque empty so send update
		HandleFetchUpdate();
	}
	
	
	//alert main thread that fetch is good as dead
	mFetchThreadAlive = false;	
	//die
	debug("thread","%s cache thread # %p is out of fetch loop . . . should die soon.\n",mCacheName,threadSelf);
	pthread_exit(NULL);
}



/*
 *	Check if a FetchUpdate needs to be sent.
 */
template < typename T,  typename U  > 
bool 
OmThreadedCache<T,U>::FetchUpdateCheck() {
	
	//get current time
	time_t current_time;
	time( &current_time );
	
	//return true and update if enough time has elapsed
	if( difftime(current_time, mLastUpdateTime) > mFetchUpdateInterval ) {
		//update last update time
		mLastUpdateTime = current_time;
		
		//clear fetch stack so that new relevant keys are requested
		pthread_mutex_lock(&mCacheMutex);
		if(mFetchUpdateClearsStack) mFetchStack.clear();
		pthread_mutex_unlock(&mCacheMutex);
		
		//return that fetch update should be called
		return true;
	}
	
	//no fetch update is needed
	return false;
}





template < typename T,  typename U  > 
bool 
OmThreadedCache<T,U>::InitializeFetchThread() { 
	return false;
}


template < typename T,  typename U  > 
void
OmThreadedCache<T,U>::SetCacheName( const char* name) {
        pthread_mutex_lock(&mCacheMutex);
	strncpy (mCacheName,name,40);
	pthread_mutex_unlock(&mCacheMutex);
	return;
}

template < typename T,  typename U  > 
void
OmThreadedCache<T,U>::GetCacheName(char * result) {
	pthread_mutex_lock(&mCacheMutex);
	strncpy (result, mCacheName, 40);
	pthread_mutex_unlock(&mCacheMutex);
	return;
}









/////////////////////////////////
///////		 Fetch Loop Initializer


/*
 *	Static function to call the FetchLoop method of the given GenericThreadedCache.
 */
template < typename T,  typename U  >
inline void*
start_cache_fetch_thread(void* arg)
{
	OmThreadedCache<T,U> *cache = (OmThreadedCache<T,U> *) arg;
     //debug("thread","Cache Type = %s
	cache->FetchLoop();
	return 0;
}

template < typename T,  typename U  > 
void
OmThreadedCache<T,U>::KillFetchLoop() {

	mKillingFetchThread = true;
	pthread_cond_signal(&mFetchThreadCv);

	//spin until done killing
	while(mFetchThreadAlive) { }
	

}

#endif
