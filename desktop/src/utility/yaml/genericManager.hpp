#pragma once
#include "precomp.h"

#include "common/common.h"

#include "system/omGenericManager.hpp"
#include "utility/yaml/omBaseTypes.hpp"
#include "utility/yaml/baseTypes.hpp"

namespace YAML {

class genericManager {
 public:
  template <typename T>
  static Node Save(const OmGenericManager<T>& gm) {
    Node n;
    n["size"] = gm.size_;
    n["valid set"] = gm.validSet_;
    n["enabled set"] = gm.enabledSet_;
    n["next id"] = gm.nextId_;

    for (auto& iter : gm.validSet_) {
      n["values"][iter] = *gm.vec_[iter];
    }
    return n;
  }

  template <typename T>
  static bool Load(const Node& in, OmGenericManager<T>& gm) {
    if (!in.IsMap()) {
      return false;
    }
    gm.size_ = in["size"].as<size_t>(0);
    gm.validSet_ = in["valid set"].as<om::common::IDSet>(om::common::IDSet());
    gm.enabledSet_ =
        in["enabled set"].as<om::common::IDSet>(om::common::IDSet());
    gm.nextId_ = in["next id"].as<om::common::ID>(0);
    gm.vec_.resize(gm.size_, nullptr);

    int idx = 0;
    for (auto& i : gm.validSet_) {
      T* t = new T(i);
      YAML::convert<T>::decode(in["values"][idx], *t);
      gm.vec_[t->GetID()] = t;

      gm.vecValidPtrs_.push_back(t);
      idx++;
    }
    return true;
  }
};

}  // namespace YAML
