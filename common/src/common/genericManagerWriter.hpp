#pragma once

#include "common/common.h"
#include "yaml-cpp/yaml.h"
#include "common/genericManager.hpp"
#include "utility/yaml/baseTypes.hpp"

namespace om {
namespace common {

template <class T>
struct GenericManagerWriter {
  static YAML::Node encode(const GenericManager<T>& gm) {
    YAML::Node node;
    node["size"] = gm.size_;
    node["valid set"] = gm.validSet_;
    node["enabled set"] = gm.enabledSet_;
    node["next id"] = gm.nextId_;
    for (auto& iter : gm.vec_) {
      node["values"] = *iter;
    }
    return node;
  }

  template <typename... TArgs>
  static bool decode(const YAML::Node& node, GenericManager<T>& gm,
                     TArgs... args) {
    if (!node.IsMap()) {
      return false;
    }

    gm.size_ = node["size"].as<uint32_t>();
    gm.validSet_ = node["valid set"].as<IDSet>();
    gm.enabledSet_ = node["enabled set"].as<IDSet>();
    gm.nextId_ = node["next id"].as<ID>();

    gm.vec_.resize(gm.size_, nullptr);

    int idx = 0;
    for (auto id : gm.enabledSet_) {
      T* t = new T(id, args...);
      gm.vec_[id] = t;
      YAML::convert<T>::decode(node["values"][idx], *t);

      gm.vecValidPtrs_.push_back(t);
      idx++;
    }
    return true;
  }
};
}
}  // namespace om::utility::
