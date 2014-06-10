#pragma once

#include "network/http/http.hpp"

namespace om {
namespace network {
namespace http {

template <typename T>
class Cache {
 public:
  typedef std::shared_ptr<GetRequest<T>> RequestType;

  template <typename Derived>
  typename std::enable_if<std::is_base_of<T, Derived>::value, RequestType>::type
  GET(const network::Uri& uri) {
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
      auto req = network::http::GET<Derived>(uri);
      {
        std::lock_guard<std::mutex> lk(m_);
        cache_[uriString] = req;
      }
      return cache_[uriString];
    }
    catch (om::Exception e) {
      log_debugs << "Failed loading task: " << e.what();
    }
    return RequestType();
  }

  RequestType GET(const network::Uri& uri) { return GET<T>(uri); }

  void Clear() { cache_.clear(); }

 private:
  std::mutex m_;
  std::unordered_map<std::string, RequestType> cache_;
};
}
}
}  // namespace om::network::