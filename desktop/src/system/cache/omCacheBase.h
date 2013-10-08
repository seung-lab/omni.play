#pragma once

/**
 * Base class of all caches.
 * Brett Warne - bwarne@mit.edu - 7/15/09
 */

#include "common/omCommon.h"

class OmCacheBase {
 protected:
  const std::string cacheName_;
  const om::CacheGroup cacheGroup_;

 public:
  OmCacheBase(const std::string& cacheName, const om::CacheGroup group)
      : cacheName_(cacheName), cacheGroup_(group) {}

  virtual ~OmCacheBase() {}

  om::CacheGroup Group() const { return cacheGroup_; }

  virtual void Clean() = 0;
  virtual void RemoveOldest(const int64_t numBytes) = 0;
  virtual void Clear() = 0;

  virtual int64_t GetCacheSize() const = 0;

  virtual void CloseDownThreads() = 0;

  virtual const std::string& GetName() const { return cacheName_; }

  friend std::ostream& operator<<(std::ostream& out, const OmCacheBase& in) {
    out << in.GetName() << " (" << in.Group() << ")";
    return out;
  }
};
