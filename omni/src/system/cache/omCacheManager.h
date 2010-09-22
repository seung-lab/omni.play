#ifndef OM_CACHE_MANAGER_H
#define OM_CACHE_MANAGER_H

#include "common/omCommon.h"
#include "utility/omLockedObjects.h"

#include <zi/mutex>
#include <zi/threads>
#include <zi/utility>

class OmCacheBase;
class OmCacheGroup;
class OmCacheInfo;
class OmPreferenceEvent;

class OmCacheManager : boost::noncopyable {
public:
	static QList< OmCacheInfo > GetCacheInfo(OmCacheGroupEnum group);
	static unsigned int Freshen(const bool freshen);
	static void AddCache(OmCacheGroupEnum group, OmCacheBase *base);
	static int  CleanCacheGroup(OmCacheGroupEnum group);
	static void RemoveCache(OmCacheGroupEnum group, OmCacheBase *base);
	static void SignalCachesToCloseDown();
	static void UpdateCacheSizeFromLocalPrefs();
	static void Delete();

	static inline boost::shared_ptr<OmCacheGroup> GetCache(const OmCacheGroupEnum group)
	{
		if(group == RAM_CACHE_GROUP){
			return Instance().mRamCacheMap;
		} else {
			return Instance().mVramCacheMap;
		}
	}

	static inline bool AmClosingDown(){
		return Instance().amClosingDown.get();
	}

protected:
	void PreferenceChangeEvent(OmPreferenceEvent *event);

private:
	OmCacheManager();
	~OmCacheManager();
	static inline OmCacheManager& Instance(){
		return zi::Singleton<OmCacheManager>::Instance();
	}

	zi::ThreadFactory threadFactory_;
	bool CacheManagerCleaner();
	LockedBool amClosingDown;

	boost::shared_ptr<OmCacheGroup> mRamCacheMap;
	boost::shared_ptr<OmCacheGroup> mVramCacheMap;
 	friend class zi::Singleton<OmCacheManager>;
};

#endif
