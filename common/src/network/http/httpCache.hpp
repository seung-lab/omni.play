#pragma once

#include "network/http/http.hpp"

namespace om {
namespace network {
namespace http {

class Cache {
 public:
  typedef std::shared_ptr<GetRequest> RequestType;
  template <typename T>
  using TypedRequestType = std::shared_ptr<TypedGetRequest<T>>;

  RequestType GET(const network::Uri& uri) {
    auto uriString = uri.string();
    auto ret = cache_get(uriString);
    if (ret) {
      return ret;
    }
    try {
      RequestType req = http::GET(uri);
      cache_store(uriString, req);
      return req;
    }
    catch (om::Exception e) {
      log_debugs << "Failed loading task: " << e.what();
    }
    return RequestType();
  }

  template <typename T>
  TypedRequestType<T> GET(const network::Uri& uri) {
    auto uriString = uri.string();
    auto ret = cache_get(uriString);
    if (ret) {
      return ret;
    }
    try {
      RequestType req = http::GET<T>(uri);
      cache_store(uriString, req);
      return req;
    }
    catch (om::Exception e) {
      log_debugs << "Failed loading task: " << e.what();
    }
    return RequestType();
  }

  void Clear() { cache_.clear(); }

 private:
  RequestType cache_get(const std::string& uriString) {
    decltype(cache_.find(uriString)) iter;
    {
      std::lock_guard<std::mutex> lk(m_);
      iter = cache_.find(uriString);
    }
    if (iter != cache_.end()) {
      return iter->second;
    }
    return RequestType();
  }

  void cache_store(const std::string& uriString, RequestType req) {
    std::lock_guard<std::mutex> lk(m_);
    cache_[uriString] = req;
  }

  std::mutex m_;
  std::unordered_map<std::string, RequestType> cache_;
};
}
}
}  // namespace om::network::