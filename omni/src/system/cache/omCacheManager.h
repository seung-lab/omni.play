#ifndef OM_CACHE_MANAGER_H
#define OM_CACHE_MANAGER_H

#include "common/omCommon.h"
#include "zi/omUtility.h"

class OmCacheBase;
class OmCacheGroup;
class OmCacheInfo;
class OmTileCache;
class OmCacheManagerImpl;

class OmCacheManager : private om::singletonBase<OmCacheManager> {
private:
	boost::shared_ptr<OmCacheManagerImpl> impl_;

public:
	static QList<OmCacheInfo> GetCacheInfo(const OmCacheGroupEnum group);
	static void AddCache(const OmCacheGroupEnum group, OmCacheBase* base);
	static void RemoveCache(const OmCacheGroupEnum group, OmCacheBase* base);

	static void SignalCachesToCloseDown();
	static void UpdateCacheSizeFromLocalPrefs();

	static void Delete();
	static void Reset();

	static void TouchFresheness();
	static uint64_t GetFreshness();

	static bool AmClosingDown();

private:
	OmCacheManager();
	~OmCacheManager(){}

 	friend class zi::singleton<OmCacheManager>;
};

#endif
