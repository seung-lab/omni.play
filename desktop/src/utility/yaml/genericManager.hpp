#pragma once

#include "common/common.h"
#include "yaml-cpp-old/yaml.h"
#include "system/omGenericManager.hpp"
#include "utility/yaml/omBaseTypes.hpp"

namespace YAMLold {

class genericManager {
 public:
  template <typename T>
  static void Save(Emitter& out, const OmGenericManager<T>& gm) {
    out << Key << "size" << Value << gm.size_;
    out << Key << "valid set" << Value << gm.validSet_;
    out << Key << "enabled set" << Value << gm.enabledSet_;
    out << Key << "next id" << Value << gm.nextId_;

    out << Key << "values" << Value << BeginSeq;
    FOR_EACH(iter, gm.validSet_) { out << *gm.vec_[*iter]; }
    out << EndSeq;
  }

  template <typename T>
  static void Load(const Node& in, OmGenericManager<T>& gm) {
    in["size"] >> gm.size_;
    in["valid set"] >> gm.validSet_;
    in["enabled set"] >> gm.enabledSet_;
    in["next id"] >> gm.nextId_;
    gm.vec_.resize(gm.size_, nullptr);

    int idx = 0;
    FOR_EACH(i, gm.validSet_) {
      T* t = new T(*i);
      in["values"][idx] >> *t;
      gm.vec_[t->GetID()] = t;

      gm.vecValidPtrs_.push_back(t);
      idx++;
    }
  }
};

}  // namespace YAMLold
