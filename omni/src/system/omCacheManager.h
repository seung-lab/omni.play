#ifndef OM_CACHE_MANAGER_H
#define OM_CACHE_MANAGER_H

#include "common/omCommon.h"

#include <QThreadPool>
#include <QMutex>

class OmCacheBase;

static const int MAX_NUM_WORKER_THREADS = 3;

//caches
enum OmCacheGroup {
	RAM_CACHE_GROUP = 1,
	VRAM_CACHE_GROUP
};

struct CacheGroupProperties {
	CacheGroupProperties() {
		//init to zero
		Size = 0;
		MaxSize = 0;
	}
	
        //current size and max size in bytes
        unsigned long Size;
	unsigned long MaxSize;

	set< OmCacheBase* > CacheSet;
};

class OmCacheInfo;
class OmPreferenceEvent;

class OmCacheManager : boost::noncopyable {

public:
	static OmCacheManager* Instance();
	static void Delete();

	static void AddCache(OmCacheGroup group, OmCacheBase *base);
	static void RemoveCache(OmCacheGroup group, OmCacheBase *base);
	static QList< OmCacheInfo > GetCacheInfo(OmCacheGroup group);
	
	static void UpdateCacheSize(OmCacheGroup group, int delta);
	static void UpdateCacheSizeFromLocalPrefs();
	static unsigned int Freshen(bool freshen);

	static bool addWorkerThread(QRunnable * runnable, int priority = 0);
	static void clearWorkerThreads();

	static int minNumberOfThreadsPerCache(){ return MAX_NUM_WORKER_THREADS; }
	
	void doUpdateCacheSizeFromLocalPrefs();
	void UpdateCacheSizeInternal(OmCacheGroup group, int delta);
	void CleanCacheGroup(OmCacheGroup group);
	void CleanCacheGroupCopy(map< OmCacheGroup, CacheGroupProperties > & copy, OmCacheGroup group);
	
protected:
	//event handling
	void PreferenceChangeEvent(OmPreferenceEvent *event);
	
        int mSavedDelta;
        bool mDelayDelta;

private:
	//singleton
	static OmCacheManager* mspInstance;
	OmCacheManager();
	~OmCacheManager();

	QThreadPool threads;

	int mMinNumOfThreadsForAllCaches;

	mutable QMutex mCacheMapMutex;
	mutable QMutex mRealCacheMapMutex;
	mutable QMutex mFreshnessMutex;
	map< OmCacheGroup, CacheGroupProperties > mCacheMap;
	map< OmCacheGroup, CacheGroupProperties > mRemoveCacheMap;
	
	//cleaning
	float mTargetRatio;
	bool mCurrentlyCleaning;
	int mThreadCount;

	void doAddCache(OmCacheGroup group, OmCacheBase *base);
};

#endif
