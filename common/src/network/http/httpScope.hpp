#pragma once

#include "network/http/http.hpp"

namespace om {
namespace network {

class HTTPScope {
 public:
  HTTPScope(HTTPScope* parent = nullptr) : parent_(parent) {
    if (parent_) {
      parent_->insert(this);
    }
  }

  ~HTTPScope() {
    if (parent_) {
      parent_->remove(this);
    }
  }

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

    {
      std::lock_guard<std::mutex> g(mut_);
      for (auto& child : children_) {
        child->Refresh();
      }
    }
  }

 private:
  void insert(HTTPScope* child) {
    std::lock_guard<std::mutex> g(mut_);
    children_.push_back(child);
  }

  void remove(HTTPScope* child) {
    std::lock_guard<std::mutex> g(mut_);
    auto iter = std::find(children_.begin(), children_.end(), child);
    if (iter != children_.end()) {
      children_.erase(iter);
    }
  }

  std::unordered_map<std::string, std::weak_ptr<IHTTPRefreshable>> cache_;

  HTTPScope* parent_;
  std::vector<HTTPScope*> children_;

  std::mutex mut_;
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
