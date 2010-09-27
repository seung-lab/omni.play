#ifndef OM_CACHE_MANAGER_H
#define OM_CACHE_MANAGER_H

#include "common/omCommon.h"
#include "utility/omLockedPODs.hpp"

#include "zi/omMutex.h"
#include "zi/omUtility.h"
#include "zi/omThreads.h"
#include <zi/concurrency/periodic_function.hpp>

class OmCacheBase;
class OmCacheGroup;
class OmCacheInfo;
class OmPreferenceEvent;
class OmTileCache;

class OmCacheManager : boost::noncopyable {
public:
	static QList<OmCacheInfo> GetCacheInfo(const OmCacheGroupEnum group);
	static void AddCache(const OmCacheGroupEnum group, OmCacheBase *base);
	static int  CleanCacheGroup(const OmCacheGroupEnum group);
	static void RemoveCache(const OmCacheGroupEnum group, OmCacheBase *base);
	static void SignalCachesToCloseDown();
	static void UpdateCacheSizeFromLocalPrefs();
	static void Delete();

	static void TouchFresheness(){
		Instance().freshness_.add(1);
	}
	static uint64_t GetFreshness(){
		return Instance().freshness_.get();
	}

	static boost::shared_ptr<OmCacheGroup>
	GetCache(const OmCacheGroupEnum group) {
		if(group == RAM_CACHE_GROUP){
			return Instance().mRamCacheMap;
		}
		return Instance().mVramCacheMap;
	}

	static bool AmClosingDown(){
		return Instance().amClosingDown.get();
	}

protected:
	void PreferenceChangeEvent(OmPreferenceEvent *event);

private:
	OmCacheManager();
	~OmCacheManager();
	static inline OmCacheManager& Instance(){
		return zi::singleton<OmCacheManager>::instance();
	}

	boost::shared_ptr<OmCacheGroup> mRamCacheMap;
	boost::shared_ptr<OmCacheGroup> mVramCacheMap;
	boost::shared_ptr<zi::periodic_function> cleaner_;
	boost::shared_ptr<zi::thread> cleanerThread_;
	LockedBool amClosingDown;
	LockedUint64 freshness_;

	bool cacheManagerCleaner();
	void setupCleanerThread();
 	friend class zi::singleton<OmCacheManager>;
};

#endif
