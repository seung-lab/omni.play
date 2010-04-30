#ifndef OM_THREADED_CACHE_H
#define OM_THREADED_CACHE_H

#include "omCacheBase.h"

#include "utility/stackSet.h"
#include "common/omException.h"
#include "common/omDebug.h"

#include <QWaitCondition>
#include <QMutex>
#include <QThread>

#include <QExplicitlySharedDataPointer>

#include <list>
using std::list;

#include <time.h>

#define OM_DEFAULT_FETCH_UPDATE_CLEARS_FETCH_STACK 0
#define OM_DEFAULT_FETCH_UPDATE_INTERVAL_SECONDS 0.5f

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
class OmThreadedCache : public OmCacheBase, public QThread
{	
 public:
	
	OmThreadedCache(OmCacheGroup group, bool initFetchThread = false);
	virtual ~OmThreadedCache();
		
	//value accessors
	void Get(QExplicitlySharedDataPointer<U> &p_value,
		 const T &key, 
		 bool blocking = false, 
		 bool exist = false);
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
	virtual bool InitializeFetchThread();
	virtual bool KillFetchThread(); 
	
	/**Name function for debugging */
	void SetCacheName(const char* name);
	void GetCacheName(char* name);

	void run();
	
protected:
	virtual U* HandleCacheMiss(const T &key) = 0;
	
private:
	//key, shared pointer value cache
	map< T, QExplicitlySharedDataPointer< U > > mCachedValuesMap;

	//least recently accessed at head of list
	list< T > mKeyAccessList;
		
	StackSet< T > mFetchStack;	//stack of keys to be fetched
	set< T > mCurrentlyFetching;	//set of keys currently being fetched
	
	bool mFetchThreadAlive;
	bool mInitializeFetchThread;
	bool mKillingFetchThread;	//note that fetch thread needs to die

	bool mFetchThreadQueuing;	//note if fetch thread is currently looping through queue
					//Clean() should be ignored while queuing so queue doesn't wipe
					//away it's own work
	bool mFetchThreadCalledClean;
	

	//mutex for cache
	QMutex mCacheMutex;

	//fetch thread
	QThread mFetchThread;
	QMutex mFetchThreadMutex;
	QWaitCondition mFetchThreadCv;

	/** name/descriptor of Cache for debugging */
	char mCacheName[40];
	void * threadSelf;
};

#endif
