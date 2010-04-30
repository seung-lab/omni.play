#ifndef OM_CACHE_MANAGER_H
#define OM_CACHE_MANAGER_H

#include "system/events/omPreferenceEvent.h"
#include "common/omStd.h"

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
	
	unsigned long MaxSize;
        //current size and max size in bytes
        unsigned long Size;
	set< OmCacheBase* > CacheSet;
};

class OmCacheManager {

public:
	
	static OmCacheManager* Instance();
	static void Delete();

	static void AddCache(OmCacheGroup group, OmCacheBase *base);
	static void RemoveCache(OmCacheGroup group, OmCacheBase *base);
	static OmCacheBase* GetCache(OmCacheGroup group, int index);
	OmCacheBase* GetCacheInternal(OmCacheGroup group, int index);
	
	static void UpdateCacheSize(OmCacheGroup group, int delta);
	static void UpdateCacheSizeFromLocalPrefs();
	
	void doUpdateCacheSizeFromLocalPrefs();
	void UpdateCacheSizeInternal(OmCacheGroup group, int delta);
	void CleanCacheGroup(OmCacheGroup group);
	void CleanCacheGroupCopy(map< OmCacheGroup, CacheGroupProperties > & copy, OmCacheGroup group);
	
protected:
	// singleton constructor, copy constructor, assignment operator protected
	OmCacheManager();
	~OmCacheManager();
	OmCacheManager(const OmCacheManager&);
	OmCacheManager& operator= (const OmCacheManager&);

	
	//event handling
	void PreferenceChangeEvent(OmPreferenceEvent *event);
	
        int mSavedDelta;
        bool mDelayDelta;

private:
	//singleton
	static OmCacheManager* mspInstance;
		
	//properties map
	QMutex mCacheMapMutex;
	QMutex mRealCacheMapMutex;
	map< OmCacheGroup, CacheGroupProperties > mCacheMap;
	map< OmCacheGroup, CacheGroupProperties > mRemoveCacheMap;
	
	//cleaning
	float mTargetRatio;
	bool mCurrentlyCleaning;
	int mThreadCount;

};

#endif
