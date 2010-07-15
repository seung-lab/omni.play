#include "omThreadedCache.h"
#include "omHandleCacheMissThreaded.h"
#include <QThreadPool>

static const int MAX_THREADS = 3;
static const int MAX_FETCHING = 200;

/**
 *	Constructor initializes and starts the fetch thread.
 */
template < typename KEY, typename PTR  >
OmThreadedCache<KEY,PTR>::OmThreadedCache(OmCacheGroup group)
	: OmCacheBase(group) 
	, mKillingFetchThread(false)
	, mLastUpdateTime(0)
{ 
	mFetchUpdateInterval = OM_DEFAULT_FETCH_UPDATE_INTERVAL_SECONDS;
	mFetchUpdateClearsStack = OM_DEFAULT_FETCH_UPDATE_CLEARS_FETCH_STACK;

	threads.setMaxThreadCount(MAX_THREADS);
	for(int i = 0; i < MAX_THREADS; ++i){
		HandleCacheMissThreaded<OmThreadedCache<KEY, PTR>, KEY, PTR>* task = 
			new HandleCacheMissThreaded<OmThreadedCache<KEY, PTR>, KEY, PTR>(this);
		
		threads.start(task);
	}
}

/**
 *	Destructor ensures fetch thread is dead before destructing cache.
 */
template < typename KEY, typename PTR  >
OmThreadedCache<KEY,PTR>::~OmThreadedCache() 
{
	closeDownThreads();
}

/*
 *	Get value from cache associated with given key.
 *	Specify if Get should block calling thread.
 *
 *	note: blocking cache does not switch threads
 */
template < typename KEY, typename PTR  > 
void OmThreadedCache<KEY,PTR>::Get(QExplicitlySharedDataPointer<PTR> &p_value,
			      const KEY &key, bool blocking) 
{
	QMutexLocker locker(&mCacheMutex);

	if(mKillingFetchThread){
		p_value = QExplicitlySharedDataPointer<PTR>();
		return;
	}
	
	//check cache
	if( mCache.contains(key) ) {
		p_value = mCache.value(key);

	} else if(blocking) {

		locker.unlock();
		OmCacheManager::Instance()->CleanCacheGroup(mCacheGroup);
		p_value = QExplicitlySharedDataPointer<PTR>(HandleCacheMiss(key));
		locker.relock();

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

			mFetchThreadCv.wakeOne();
		}
	}
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

	mKeyAccessList.removeAll(key);

	QExplicitlySharedDataPointer<PTR> old_value = mCache.value(key);
	mCache.remove(key);

	locker.unlock();
	old_value.reset();
}

/**
 *	Removes oldest value from the cache.
 */
template < typename KEY, typename PTR  > 
void OmThreadedCache<KEY,PTR>::RemoveOldest() 
{
	QMutexLocker locker( &mCacheMutex );

	if( mCache.empty() ||
	    mKeyAccessList.size() < 20){
		return;
	}
	
	KEY& r_oldest_key = mKeyAccessList.back();
	mKeyAccessList.pop_back();
	
	QExplicitlySharedDataPointer<PTR> old_value = mCache.value(r_oldest_key);
	mCache.remove(r_oldest_key);

	locker.unlock();
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
unsigned int OmThreadedCache<KEY,PTR>::GetFetchStackSize()
{
	return mFetchStack.size();
}

// Get Estimate of RAM size occupied by this cache
template < typename KEY, typename PTR  >
long OmThreadedCache<KEY,PTR>::GetCacheSize()
{
        return mCache.size() * mObjectSize;
}

template < typename KEY, typename PTR  >
void OmThreadedCache<KEY,PTR>::SetObjectSize(long size)
{
        mObjectSize = size;
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
void OmThreadedCache<KEY,PTR>::closeDownThreads()
{
	mCacheMutex.lock();
	mKillingFetchThread = true;
	mCacheMutex.unlock();

	//send signal to kill fetch threads
	mFetchThreadCv.wakeAll();

	threads.waitForDone();
}

