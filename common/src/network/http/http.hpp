#pragma once

#include "precomp.h"
#include "common/logging.h"
#include "utility/resourcePool.hpp"
#include "datalayer/paths.hpp"
#include "zi/utility.h"
#include "network/uri.hpp"
#include "network/curlCore.h"
#include "network/http/requests.h"

#include "yaml-cpp/yaml.h"

namespace om {
namespace network {
namespace http {
class HTTP : private SingletonBase<HTTP> {
 public:
  template <typename T>
  static std::shared_ptr<TypedGetRequest<T>> GET(const network::Uri& uri) {
    return instance().core_.CreateRequest<TypedGetRequest<T>>(uri);
  }

  static std::shared_ptr<GetRequest> GET(const network::Uri& uri) {
    return instance().core_.CreateRequest<GetRequest>(uri);
  }

  template <typename T>
  static std::shared_ptr<PutRequest> PUT(const network::Uri& uri,
                                         const T& data) {
    return instance().core_.CreateRequest<TypedPutRequest<T>>(uri, data);
  }

  template <typename... Ts>
  static std::shared_ptr<PostRequest> POST(const network::Uri& uri,
                                           Ts&&... posts) {
    return instance().core_.CreateRequest<PostRequest>(
        uri, std::forward<Ts>(posts)...);
  }

 private:
  CURLCore core_;

  friend class zi::singleton<HTTP>;
};

template <typename T>
static std::shared_ptr<TypedGetRequest<T>> GET(const network::Uri& uri) {
  return HTTP::GET<T>(uri);
}

static std::shared_ptr<GetRequest> GET(const network::Uri& uri) {
  return HTTP::GET(uri);
}

template <typename T>
static std::shared_ptr<PutRequest> PUT(const network::Uri& uri, const T& data) {
  return HTTP::PUT<T>(uri, data);
}

template <typename... Ts>
static std::shared_ptr<PostRequest> POST(const network::Uri& uri,
                                         Ts&&... posts) {
  return HTTP::POST(uri, std::forward<Ts>(posts)...);
}
}
}
}