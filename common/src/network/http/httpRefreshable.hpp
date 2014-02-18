#pragma once

#include <string>
#include "yaml-cpp/yaml.h"
#include "common/logging.h"

namespace om {
namespace network {

class IHTTPRefreshable {
 public:
  virtual void Refresh(const std::string&) = 0;
};

template <typename TData>
class HTTPRefreshable : public IHTTPRefreshable {
 public:
  virtual ~HTTPRefreshable() {}
  virtual void Refresh(const std::string& data) {
    if (!data.size()) {
      return;
    }

    try {
      auto node = YAML::Load(data);
      data_ = node.as<TData>();
    }
    catch (YAML::Exception e) {
      log_debugs << "Failed loading JSON: " << e.what();
    }
  }

 protected:
  TData data_;
};

template <typename T>
class HTTPRefreshablePOD : public IHTTPRefreshable {
 public:
  virtual ~HTTPRefreshablePOD() {}
  virtual void Refresh(const std::string& data) {
    if (!data.size()) {
      return;
    }

    try {
      auto node = YAML::Load(data);
      YAML::convert<T>::decode(node, *this);
    }
    catch (YAML::Exception e) {
      log_debugs << "Failed loading JSON: " << e.what();
    }
  }
};

template <typename T>
class HTTPRefreshableType : public IHTTPRefreshable, public T {
 public:
  virtual ~HTTPRefreshableType() {}
  virtual void Refresh(const std::string& data) {
    if (!data.size()) {
      return;
    }

    try {
      auto node = YAML::Load(data);
      YAML::convert<T>::decode(node, *this);
    }
    catch (YAML::Exception e) {
      log_debugs << "Failed loading JSON: " << e.what();
    }
  }
};
}
}  // namespace om::network::
