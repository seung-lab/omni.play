#pragma once

#include "tiles/omTileCoord.h"
#include "zi/omMutex.h"

class OmTilesToPrefetch {
 private:
  typedef OmTileCoord key_t;

  struct PrefetchKey {
    int depthOffset;
    key_t key;

    bool operator<(const PrefetchKey& a) const {
      if (std::abs(depthOffset) == std::abs(a.depthOffset)) {
        if (key == a.key) {
          return false;
        }

        return key < a.key;
      }

      return std::abs(depthOffset) < std::abs(a.depthOffset);
    }
  };

  std::multiset<PrefetchKey> prefetchKeys_;
  zi::spinlock prefetchKeysLock_;

 public:
  void insert(const key_t& key, const int depthOffset) {
    PrefetchKey pf = { depthOffset, key };
    {
      zi::guard g(prefetchKeysLock_);
      prefetchKeys_.insert(pf);
    }
  }

  key_t get_front() {
    zi::guard g(prefetchKeysLock_);

    PrefetchKey pf = *prefetchKeys_.begin();
    prefetchKeys_.erase(prefetchKeys_.begin());

    return pf.key;
  }
};
