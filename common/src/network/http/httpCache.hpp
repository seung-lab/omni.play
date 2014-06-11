#pragma once

#include "network/http/http.hpp"

namespace om {
namespace network {
namespace http {

class Cache {
 public:
  typedef std::shared_ptr<GetRequest> RequestType;

  RequestType GET(const network::Uri& uri) { return do_get<RequestType>(uri); }

  template <typename T>
  std::shared_ptr<TypedGetRequest<T>> GET(const network::Uri& uri) {
    return std::dynamic_pointer_cast<TypedGetRequest<T>>(
        do_get<TypedGetRequest<T>>(uri));
  }

  void Clear() { cache_.clear(); }

 private:
  template <typename T>
  RequestType cached_get(const network::Uri& uri) {
    auto uriString = uri.string();
    decltype(cache_.find(uriString)) iter;
    {
      std::lock_guard<std::mutex> lk(m_);
      iter = cache_.find(uriString);
    }
    if (iter != cache_.end()) {
      return iter->second;
    }
    try {
      RequestType req = do_get<T>(uri);
      {
        std::lock_guard<std::mutex> lk(m_);
        cache_[uriString] = req;
      }
      return req;
    }
    catch (om::Exception e) {
      log_debugs << "Failed loading task: " << e.what();
    }
    return RequestType();
  }

  template <typename T>
  RequestType do_get(typename std::enable_if<
      std::is_same<T, RequestType>::value, const network::Uri&>::type uri) {
    return network::http::GET(uri);
  }

  template <typename T, typename U>
  RequestType do_get(
      typename std::enable_if<!std::is_same<T, TypedGetRequest<U>>::value,
                              const network::Uri&>::type uri) {
    return network::http::GET<U>(uri);
  }

  std::mutex m_;
  std::unordered_map<std::string, RequestType> cache_;
};
}
}
}  // namespace om::network::