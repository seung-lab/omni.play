#pragma once

#include "network/http/http.hpp"

namespace om {
namespace network {

template <typename T>
class HTTPCache {
 public:
  template <typename Derived>
  typename std::enable_if<std::is_base_of<T, Derived>::value,
                          std::shared_ptr<T>>::type
  GET(const network::Uri& uri) {
    auto uriString = uri.string();
    auto iter = cache_.find(uriString);
    if (iter != cache_.end()) {
      return iter->second;
    }
    try {
      cache_[uriString] = network::HTTP::GET_JSON<Derived>(uriString);
      return cache_[uriString];
    }
    catch (om::Exception e) {
      log_debugs << "Failed loading task: " << e.what();
    }
    return std::shared_ptr<T>();
  }

  std::shared_ptr<T> GET(const network::Uri& uri) { return GET<T>(uri); }

  void Clear() { cache_.clear(); }

 private:
  std::unordered_map<std::string, std::shared_ptr<T>> cache_;
};
}
}  // namespace om::network::