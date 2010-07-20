#ifndef OM_CACHE_MANAGER_H
#define OM_CACHE_MANAGER_H

#include "common/omCommon.h"

#include <QReadWriteLock>
#include <QMutex>

class OmCacheBase;

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

	static void SignalCachesToCloseDown();

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

	mutable QReadWriteLock mCacheMutex;

	mutable QMutex mFreshnessMutex;
	CacheGroupProperties mRamCacheMap;
	CacheGroupProperties mVramCacheMap;
	
	//cleaning
	float mTargetRatio;
	bool mCurrentlyCleaning;
	int mThreadCount;
};

#endif
