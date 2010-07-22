#ifndef OM_THREADED_CACHE_H
#define OM_THREADED_CACHE_H

#include "omCacheBase.h"

#include "common/omException.h"
#include "common/omDebug.h"

#include <QThreadPool>
#include <QWaitCondition>
#include <QMutex>
#include <QThread>
#include <QExplicitlySharedDataPointer>
#include <QStack>
#include <QMap>
#include <QSemaphore>
#include <time.h>

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
class OmThreadedCache : public OmCacheBase {	
 public:
	OmThreadedCache(OmCacheGroup group);
	virtual ~OmThreadedCache();
	
	//value accessors
	void Get(QExplicitlySharedDataPointer<PTR> &p_value,
		 const KEY &key, 
		 bool blocking);
	void Remove(const KEY &key);
	void RemoveOldest();
	void Flush();

	//get info about the cache 
	unsigned int GetFetchStackSize();
	long GetCacheSize();
        void SetObjectSize(long size);

	virtual PTR* HandleCacheMiss(const KEY &key) = 0;

	// FIXME: don't make public...
	mutable QMutex mCacheMutex;
	bool mKillingFetchThread;	//note that fetch thread needs to die
	QStack< KEY > mFetchStack;	 // keys to be fetched
	QList< KEY > mCurrentlyFetching; // keys currently being fetched
	QWaitCondition mFetchThreadCv;
	QMap< KEY, QExplicitlySharedDataPointer< PTR > > mCache;

	//least recently accessed at head of list
	QList< KEY > mKeyAccessList;

	void closeDownThreads();
	
private:
	QThreadPool threads;

        // size of objects in this cache
        long mObjectSize;
};

#endif
