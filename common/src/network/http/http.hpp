#pragma once

#include "precomp.h"
#include "common/logging.h"
#include "utility/resourcePool.hpp"
#include "datalayer/paths.hpp"
#include "zi/utility.h"
#include "network/uri.hpp"

#include "yaml-cpp/yaml.h"

namespace om {
namespace network {
namespace http {
class HTTP : private SingletonBase<HTTP> {
 public:
  template <typename T>
  static std::shared_ptr<GetRequest<T>> GET(const network::Uri& uri) {
    return instance().core_.CreateRequest<GetRequest<T>>(uri);
  }

  template <typename T>
  static std::shared_ptr<PutRequest<T>> PUT(const network::Uri& uri,
                                            const T& data) {
    return instance().core_.CreateRequest<PutRequest<T>>(uri, data);
  }

  template <typename... Ts>
  static std::shared_ptr<PostRequest<T>> POST(const network::Uri& uri,
                                              Ts&&... posts) {
    return instance().core_.CreateRequest<PostRequest<T>>(
        uri, std::forward<Ts>(posts)...);
  }

 private:
  CURLCore core_;

  friend class zi::singleton<HTTP>;
};
}

#undef SET_OPT
}
}