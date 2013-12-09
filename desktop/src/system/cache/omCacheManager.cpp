#include "system/cache/omCacheManager.h"
#include "system/cache/omCacheManagerImpl.hpp"

OmCacheManager::OmCacheManager() {}

OmCacheManager::~OmCacheManager() {}

void OmCacheManager::Delete() { instance().impl_.reset(); }

void OmCacheManager::Reset() {
  instance().impl_.reset(new OmCacheManagerImpl());
}

void OmCacheManager::UpdateCacheSizeFromLocalPrefs() {
  impl()->UpdateCacheSizeFromLocalPrefs();
}

void OmCacheManager::AddCache(const om::common::CacheGroup group,
                              OmCacheBase* base) {
  impl()->AddCache(group, base);
}

void OmCacheManager::RemoveCache(const om::common::CacheGroup group,
                                 OmCacheBase* base) {
  impl()->RemoveCache(group, base);
}

std::vector<OmCacheInfo> OmCacheManager::GetCacheInfo(
    const om::common::CacheGroup group) {
  return impl()->GetCacheInfo(group);
}

void OmCacheManager::SignalCachesToCloseDown() {
  impl()->SignalCachesToCloseDown();
}

void OmCacheManager::TouchFreshness() { impl()->TouchFreshness(); }

uint64_t OmCacheManager::GetFreshness() { return impl()->GetFreshness(); }

bool OmCacheManager::AmClosingDown() { return impl()->AmClosingDown(); }

void OmCacheManager::ClearCacheContents() { impl()->ClearCacheContents(); }
