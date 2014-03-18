#pragma once
#include "precomp.h"

#include "common/common.h"
#include "common/enums.hpp"
#include "zi/omUtility.h"

class OmCacheBase;
class OmCacheGroup;
class OmCacheInfo;
class OmTileCache;
class OmCacheManagerImpl;

class OmCacheManager : private om::singletonBase<OmCacheManager> {
 private:
  std::unique_ptr<OmCacheManagerImpl> impl_;

  inline static OmCacheManagerImpl* impl() { return instance().impl_.get(); }

 public:
  static std::vector<OmCacheInfo> GetCacheInfo(
      const om::common::CacheGroup group);
  static void AddCache(const om::common::CacheGroup group, OmCacheBase* base);
  static void RemoveCache(const om::common::CacheGroup group,
                          OmCacheBase* base);

  static void SignalCachesToCloseDown();
  static void UpdateCacheSizeFromLocalPrefs();

  static void Delete();
  static void Reset();

  static void TouchFreshness();
  static uint64_t GetFreshness();

  static void ClearCacheContents();

  static bool AmClosingDown();

 private:
  OmCacheManager();
  ~OmCacheManager();

  friend class zi::singleton<OmCacheManager>;
};
