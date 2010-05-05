#ifndef OM_THREADED_CACHE_H
#define OM_THREADED_CACHE_H

#include "omCacheBase.h"

#include "common/omException.h"
#include "common/omDebug.h"

#include <QWaitCondition>
#include <QMutex>
#include <QThread>
#include <QExplicitlySharedDataPointer>
#include <QStack>

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

template < typename KEY, typename PTR  >
class OmThreadedCache : public OmCacheBase, public QThread
{	
 public:
	
	OmThreadedCache(OmCacheGroup group, bool initFetchThread = false);
	virtual ~OmThreadedCache();
		
	//value accessors
	void Get(QExplicitlySharedDataPointer<PTR> &p_value,
		 const KEY &key, 
		 bool blocking);
	void Add(const KEY &key, PTR *value);
	void Remove(const KEY &key);
	void RemoveOldest();
	bool Contains(const KEY &key);
	void Clear();
	void Flush();

	//fetch thread
	bool IsFetchStackEmpty();
	void FetchLoop();
	bool FetchUpdateCheck();
	virtual void HandleFetchUpdate() { };
	virtual bool InitializeFetchThread();
	virtual bool KillFetchThread(); 
	
	//fetch properties
	void SetFetchUpdateInterval(float);
	float GetFetchUpdateInterval();
	void SetFetchUpdateClearsFetchStack(bool);
	bool GetFetchUpdateClearsFetchStack();

	//get info about the cache 
	unsigned int GetFetchStackSize();
	long GetCacheSize();
        void SetObjectSize(long size);

	/**Name function for debugging */
	void SetCacheName(const char* name);
	char* GetCacheName();

	void run();
	
protected:
	virtual PTR* HandleCacheMiss(const KEY &key) = 0;
	
private:
	//key, shared pointer value cache
	QHash< KEY, QExplicitlySharedDataPointer< PTR > > mCache;

	//least recently accessed at head of list
	QList< KEY > mKeyAccessList;
		
	QStack< KEY > mFetchStack;	//stack of keys to be fetched
	QList< KEY > mCurrentlyFetching;	//set of keys currently being fetched
	
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
	
	//fetch thread update
	time_t mLastUpdateTime;
	
	//fetch update prefs
	float mFetchUpdateInterval;
	bool mFetchUpdateClearsStack;

        // size of objects in this cache
        long mObjectSize;

	/** name/descriptor of Cache for debugging */
	char mCacheName[40];
	void * threadSelf;


};

#endif
