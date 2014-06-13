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
    return instance().core_.CreateRequest<TypedGetRequest<T>>(
        uri, instance().cookies_);
  }

  static std::shared_ptr<GetRequest> GET(const network::Uri& uri) {
    return instance().core_.CreateRequest<GetRequest>(uri, instance().cookies_);
  }

  static std::shared_ptr<PutRequest> PUT(const network::Uri& uri,
                                         const std::string& data) {
    return instance().core_.CreateRequest<PutRequest>(uri, data,
                                                      instance().cookies_);
  }

  template <typename T>
  static std::shared_ptr<PutRequest> PUT(const network::Uri& uri,
                                         const T& data) {
    return instance().core_.CreateRequest<TypedPutRequest<T>>(
        uri, data, instance().cookies_);
  }

  template <typename... Ts>
  static std::shared_ptr<PostRequest> POST(const network::Uri& uri,
                                           Ts&&... posts) {
    return instance().core_.CreateRequest<PostRequest>(
        uri, instance().cookies_, std::forward<Ts>(posts)...);
  }

  static void SetDefaultCookies(std::shared_ptr<curl_slist> cookies) {
    instance().cookies_ = cookies;
  }

 private:
  CURLCore core_;
  std::shared_ptr<curl_slist> cookies_;

  friend class zi::singleton<HTTP>;
};

template <typename T>
static std::shared_ptr<TypedGetRequest<T>> GET(const network::Uri& uri) {
  return HTTP::GET<T>(uri);
}

inline static std::shared_ptr<GetRequest> GET(const network::Uri& uri) {
  return HTTP::GET(uri);
}

template <typename T>
static std::shared_ptr<PutRequest> PUT(const network::Uri& uri, const T& data) {
  return HTTP::PUT<T>(uri, data);
}

inline static std::shared_ptr<PutRequest> PUT(const network::Uri& uri,
                                              const std::string& data) {
  return HTTP::PUT(uri, data);
}

template <typename... Ts>
static std::shared_ptr<PostRequest> POST(const network::Uri& uri,
                                         Ts&&... posts) {
  return HTTP::POST(uri, std::forward<Ts>(posts)...);
}

inline static void SetDefaultCookies(const HTTPRequest& req) {
  HTTP::SetDefaultCookies(req.cookies());
}
}
}
}