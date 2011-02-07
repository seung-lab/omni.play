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
    boost::scoped_ptr<OmCacheManagerImpl> impl_;

    inline static OmCacheManagerImpl* impl(){
        return instance().impl_.get();
    }

public:
    static QList<OmCacheInfo> GetCacheInfo(const om::CacheGroup group);
    static void AddCache(const om::CacheGroup group, OmCacheBase* base);
    static void RemoveCache(const om::CacheGroup group, OmCacheBase* base);

    static void SignalCachesToCloseDown();
    static void UpdateCacheSizeFromLocalPrefs();

    static void Delete();
    static void Reset();

    static void TouchFresheness();
    static uint64_t GetFreshness();

    static void ClearCacheContents();

    static bool AmClosingDown();

private:
    OmCacheManager();
    ~OmCacheManager();

    friend class zi::singleton<OmCacheManager>;
};

#endif
