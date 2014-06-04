#pragma once

#include "precomp.h"
#include "common/logging.h"
#include "utility/resourcePool.hpp"

namespace om {
namespace network {

struct CurlHandle {
  CurlHandle() { Handle = curl_easy_init(); }
  ~CurlHandle() {
    if (Handle != nullptr) {
      curl_easy_cleanup(Handle);
    }
  }

  static void ResetHandle(std::shared_ptr<CurlHandle> h) {
    if ((bool)h) {
      curl_easy_reset(h->Handle);
    }
  }

  CURL* Handle;
};
typedef utility::ResourcePool<CurlHandle> handle_pool;
}
}  // namespace om::network::