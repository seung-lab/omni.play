
#ifndef OM_CACHE_MANAGER_H
#define OM_CACHE_MANAGER_H

/*
 *
 */

#include "system/events/omPreferenceEvent.h"

#include "common/omSerialization.h"
#include "common/omThreads.h"
#include "common/omStd.h"


//#define DEFAULT_MAX_CACHE_SIZE_BYTES (128 * BYTES_PER_MB)


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
	pthread_t mRemoveThread;
	set< OmCacheBase* > CacheSet;
};




class OmCacheManager : public OmPreferenceEventListener {

public:
	
	static OmCacheManager* Instance();
	static void Delete();

	static void AddCache(OmCacheGroup group, OmCacheBase *base);
	static void RemoveCache(OmCacheGroup group, OmCacheBase *base);
	
	static void UpdateCacheSize(OmCacheGroup group, int delta);
	
	void UpdateCacheSizeInternal(OmCacheGroup group, int delta);
	void CleanCacheGroup(OmCacheGroup group);
	void CleanCacheGroupCopy(map< OmCacheGroup, CacheGroupProperties > & copy, OmCacheGroup group);

	static void * CleanOne (void* in);		// Threaded function.
	
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
	pthread_mutex_t mCacheMapMutex;
	pthread_mutex_t mRealCacheMapMutex;
	map< OmCacheGroup, CacheGroupProperties > mCacheMap;
	map< OmCacheGroup, CacheGroupProperties > mRemoveCacheMap;
	
	//cleaning
	float mTargetRatio;
	bool mCurrentlyCleaning;
	int mThreadCount;
	
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int file_version);
};







#pragma mark 
#pragma mark Serialization
/////////////////////////////////
///////		 Serialization


BOOST_CLASS_VERSION(OmCacheManager, 0)

template<class Archive>
void 
OmCacheManager::serialize(Archive & ar, const unsigned int file_version) {

}



#endif
