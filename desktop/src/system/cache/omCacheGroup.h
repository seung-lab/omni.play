#pragma once

#include "common/common.h"
#include "zi/omMutex.h"

#include <QList>

class OmCacheBase;
class OmCacheInfo;

class OmCacheGroup {
 public:
  OmCacheGroup(const om::common::CacheGroup cacheGroup);

  void AddCache(OmCacheBase* cache);
  void RemoveCache(OmCacheBase* cache);
  void DeleteCaches();

  void SetMaxSizeMB(const int64_t size);

  void Clean();
  void ClearCacheContents();
  void SignalCachesToCloseDown();

  std::vector<OmCacheInfo> GetCacheInfo();

 private:
  const om::common::CacheGroup cacheGroup_;
  int64_t maxAllowedSize_;

  zi::rwmutex lock_;
  std::set<OmCacheBase*> caches_;

  void clearDeadItems();
  int64_t currentSize();
  int64_t removeOldest(const int64_t currentSize);
};
