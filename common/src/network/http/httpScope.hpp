#pragma once

#include "network/http/http.hpp"

namespace om {
namespace network {

class HTTPScope {
 public:
  template <typename T>
  typename std::enable_if<std::is_base_of<IHTTPRefreshable, T>::value,
                          std::shared_ptr<T>>::type
  GET(const std::string& uri) {
    auto cached = cache_.find(uri);
    if (cached != cache_.end()) {
      auto ptr = std::dynamic_pointer_cast<T>(cached->second.lock());
      if ((bool)ptr) {
        return ptr;
      } else {
        cache_.erase(cached);
      }
    }

    auto fetched = HTTP::GET<T>(uri);

    cache_[uri] = fetched;

    return fetched;
  }

  void Refresh() {
    std::vector<std::string> dead;
    for (auto& cached : cache_) {
      auto ptr = cached.second.lock();
      if ((bool)ptr) {
        ptr->Refresh(HTTP::GET(cached.first));
      } else {
        dead.push_back(cached.first);
      }
    }

    for (auto& d : dead) {
      cache_.erase(d);
    }
  }

 private:
  std::unordered_map<std::string, std::weak_ptr<IHTTPRefreshable>> cache_;
};

class ApplicationScope : private SingletonBase<ApplicationScope> {
 public:
  template <typename T>
  static std::shared_ptr<T> GET(const std::string& uri) {
    return instance().scope_.GET<T>(uri);
  }

  static void Refresh() { instance().scope_.Refresh(); }

 private:
  HTTPScope scope_;
  friend class zi::singleton<HTTP>;
};
}
}
