#include "omThreadedCache.h"
#include "omHandleCacheMissThreaded.h"
#include <QThreadPool>

#define MAX_FETCHING (200)

/**
 *	Constructor initializes and starts the fetch thread.
 */
template < typename KEY, typename PTR  >
OmThreadedCache<KEY,PTR>::OmThreadedCache(OmCacheGroup group)
	: OmCacheBase(group) 
{ 
	//fetch prefs
	mFetchUpdateInterval = OM_DEFAULT_FETCH_UPDATE_INTERVAL_SECONDS;
	mFetchUpdateClearsStack = OM_DEFAULT_FETCH_UPDATE_CLEARS_FETCH_STACK;
	
	//init state variables
	mLastUpdateTime = 0;
	mKillingFetchThread = false;

	start();
}

/**
 *	Destructor ensures fetch thread is dead before destructing cache.
 */
template < typename KEY, typename PTR  >
OmThreadedCache<KEY,PTR>::~OmThreadedCache() 
{
	//send signal to kill fetch thread
	mKillingFetchThread = true;
	mFetchThreadCv.wakeAll();

	wait();

	OmCacheManager::clearWorkerThreads();
}

/////////////////////////////////
///////		 Value Accessors


/*
 *	Get value from cache associated with given key.
 *	Specify if Get should block calling thread.
 *
 *	note: blocking chace does not switch threads
 */
template < typename KEY, typename PTR  > 
void 
OmThreadedCache<KEY,PTR>::Get(QExplicitlySharedDataPointer<PTR> &p_value,
			      const KEY &key, bool blocking) 
{
  	OmCacheManager::Instance()->CleanCacheGroup(mCacheGroup);
	
	mCacheMutex.lock();
	
	//check cache
	if( mCache.contains(key) ) {
		p_value = mCache.value(key);

	} else if(blocking) {

		mCacheMutex.unlock();

		p_value = QExplicitlySharedDataPointer<PTR>(HandleCacheMiss(key));

		mCacheMutex.lock();

		mCache[key] = p_value;
		mKeyAccessList.push_front(key);
				
	} else {
		//not in cache so return null pointer
		p_value = QExplicitlySharedDataPointer<PTR>();
		
		//if not already in stack and not currently being fetched
		if( !mFetchStack.contains(key)        && 
		    !mCurrentlyFetching.contains(key) ){

			if (mFetchStack.size () > MAX_FETCHING) {
				mFetchStack.clear();
			}

			mFetchStack.push(key);

			//signal fetch thread
			mFetchThreadCv.wakeAll();
		}			
	}

	mCacheMutex.unlock();
}

/**
 *	Add key value pair to cache.
 *
 *	Although Add does not increase the size of the cache, it does
 *	destruct a value.  This destructor may call UpdateSize() which
 *	may try to clean the cache.  This means we need to destruct the
 *	value outside of the cache lock.
 */
template < typename KEY, typename PTR  > 
void OmThreadedCache<KEY,PTR>::Add(const KEY &key, PTR *value) 
{
	QMutexLocker locker( &mCacheMutex );

	//if there was an older vaue with same key
	QExplicitlySharedDataPointer<PTR> old_value;
	
	//if already in cache
	if( mCache.contains(key) ) {
		
		//get ref to old value
		old_value = mCache.value(key);
		
		//then remove from cache
		mCache.remove(key);
		mKeyAccessList.removeAll(key);
	}
	
	//add to cache map
	mCache[key] = QExplicitlySharedDataPointer<PTR>(value);

	//add to access list
	mKeyAccessList.push_front(key);
	
	old_value.reset();
}

/**
 *	Removes value from the cache.
 *
 *	throws: modification exception if key is in use
 */
template < typename KEY, typename PTR  > 
void OmThreadedCache<KEY,PTR>::Remove(const KEY &key) 
{	
	QMutexLocker locker( &mCacheMutex );

	QExplicitlySharedDataPointer<PTR> old_value = mCache.value(key);

	//then remove from cache
	mCache.remove(key);
	mKeyAccessList.removeAll(key);
	
	old_value.reset();
}

/**
 *	Removes oldest value from the cache.
 *
 *	throws: if in use, the key is moved to head of access list
 */
template < typename KEY, typename PTR  > 
void OmThreadedCache<KEY,PTR>::RemoveOldest() 
{
	QMutexLocker locker( &mCacheMutex );

	//so as to destroy value outside of mutex
	QExplicitlySharedDataPointer<PTR> old_value = QExplicitlySharedDataPointer<PTR> ();
	
	if( mCache.empty() ) {
		return;
	}
	
	if (mKeyAccessList.size() > 20) {
		//get oldest key
		KEY& r_oldest_key = mKeyAccessList.back();
	
		//get ref to old value
		old_value = mCache.value(r_oldest_key);

		assert(r_oldest_key == mKeyAccessList.back());
	
		//then remove oldest from cache
		mCache.remove(r_oldest_key);

		//remove oldest from access list
		mKeyAccessList.pop_back();
	}

	old_value.reset();
}

/**
 *	Checks if value is in cache.
 */
template < typename KEY, typename PTR  > 
bool OmThreadedCache<KEY,PTR>::Contains(const KEY &key) 
{
	QMutexLocker locker( &mCacheMutex );
	return mCache.contains(key);
}

/**
 *	Clear all elements from the cache
 */
template < typename KEY, typename PTR > 
void OmThreadedCache<KEY,PTR>::Clear() 
{
	printf("FIXME: OmThreadedCache<KEY,PTR>::Clear()\n");
	//FIXME: this is all broken! must be, at least, mutex protected...
	//while map contains values
	//while(mCache.size()) 
	//	RemoveOldest();
}

/////////////////////////////////
///////		 Fetch Properties

template < typename KEY, typename PTR  >
void OmThreadedCache<KEY,PTR>::SetFetchUpdateInterval(float interval) 
{
	mFetchUpdateInterval = interval;
}

template < typename KEY, typename PTR  >
float OmThreadedCache<KEY,PTR>::GetFetchUpdateInterval() 
{
	return mFetchUpdateInterval; 
}

template < typename KEY, typename PTR  >
void OmThreadedCache<KEY,PTR>::SetFetchUpdateClearsFetchStack(bool state) 
{
	mFetchUpdateClearsStack = state;
}

template < typename KEY, typename PTR  >
bool OmThreadedCache<KEY,PTR>::GetFetchUpdateClearsFetchStack() 
{
	return mFetchUpdateClearsStack;
}


/////////////////////////////////
///////		 Fetching

template < typename KEY, typename PTR  >
unsigned int
OmThreadedCache<KEY,PTR>::GetFetchStackSize()
{
	return mFetchStack.size();
}

// Get Estimate of RAM size occupied by this cache
template < typename KEY, typename PTR  >
long
OmThreadedCache<KEY,PTR>::GetCacheSize()
{
        return mCache.size() * mObjectSize;
}

template < typename KEY, typename PTR  >
void
OmThreadedCache<KEY,PTR>::SetObjectSize(long size)
{
        mObjectSize = size;
}

/**
 *	This loop is only performed by the Fetch Thread.  It waits on the
 *	conditional variable triggered by the main thread when a new element
 *	is added to the deque, and loops until the deque is empty.
 */
template < typename KEY, typename PTR  >
void OmThreadedCache<KEY,PTR>::FetchLoop() 
{
	QMutexLocker bigLocker(&mCacheMutex);

	while(true) {

		if(mKillingFetchThread) {
			return;
		}
		mFetchThreadCv.wait(&mCacheMutex);
		if(mKillingFetchThread) {
			return;
		}
		
		while(!mFetchStack.empty()){
			if(mKillingFetchThread) {
				return;
			}
			
			if(mFetchStack.empty()) {
				break;
			}
			
			KEY fetch_key = mFetchStack.top();
			mFetchStack.pop();
			mCurrentlyFetching.append(fetch_key);
			
			spawnWorkerThread(fetch_key);
			
		}
	}
}

template < typename KEY, typename PTR  > 
void OmThreadedCache<KEY,PTR>::HandleFetchUpdate(KEY fetch_key, PTR * fetch_value) 
{
	QMutexLocker lock(&mCacheMutex);

	if(mKillingFetchThread){
		return;
	}

	//add to cache map
	mCache[fetch_key] = QExplicitlySharedDataPointer<PTR>(fetch_value);

	//add to access list
	mKeyAccessList.push_front(fetch_key);

	//key has been fetched, so remove from currently fetching
	mCurrentlyFetching.removeAt(mCurrentlyFetching.indexOf(fetch_key));

	lock.unlock();

	//send update if needed
	if(FetchUpdateCheck()) {
		HandleFetchUpdate();
	}
}


/**
 *	Check if a FetchUpdate needs to be sent.
 */
template < typename KEY, typename PTR  > 
bool 
OmThreadedCache<KEY,PTR>::FetchUpdateCheck() {
	
	//get current time
	time_t current_time;
	time( &current_time );
	
	//return true and update if enough time has elapsed
	if( difftime(current_time, mLastUpdateTime) > mFetchUpdateInterval ) {
		//update last update time
		mLastUpdateTime = current_time;
		
		//clear fetch stack so that new relevant keys are requested
		mCacheMutex.lock();
		if(mFetchUpdateClearsStack) mFetchStack.clear();
		mCacheMutex.unlock();
		
		//return that fetch update should be called
		return true;
	}
	
	//no fetch update is needed
	return false;
}

template < typename KEY, typename PTR  >
void OmThreadedCache<KEY,PTR>::run()
{
	FetchLoop();
}

template < typename KEY, typename PTR  >
void OmThreadedCache<KEY,PTR>::Flush() 
{
	QMutexLocker locker( &mCacheMutex );
         
	foreach( QExplicitlySharedDataPointer< PTR > ptr, mCache ){
		ptr->Flush();
	}
}

template < typename KEY, typename PTR  >
void OmThreadedCache<KEY,PTR>::spawnWorkerThread(KEY fetch_key) 
{
	HandleCacheMissThreaded<OmThreadedCache<KEY, PTR>, KEY, PTR>* thread = 
		new HandleCacheMissThreaded<OmThreadedCache<KEY, PTR>, KEY, PTR>(this, fetch_key);

	//thread->run(); //doesn't actually start thread
	//QThreadPool::globalInstance()->start(thread, QThread::LowestPriority);
	OmCacheManager::addWorkerThread(thread, QThread::LowestPriority);
}

template < typename KEY, typename PTR  >
bool OmThreadedCache<KEY,PTR>::shouldThreadDie()
{
	QMutexLocker lock(&mCacheMutex);
	return mKillingFetchThread;
}
