#pragma once
#include "precomp.h"

#include "view3d/mesh/drawer/percDone.hpp"
#include "common/string.hpp"

namespace om {
namespace v3d {

class DrawStatus {
 private:
  std::list<PercDone> percDone_;

 public:
  void Reset() { percDone_.clear(); }

  void Add(const PercDone& pd) { percDone_.push_back(pd); }

  std::string Msg() {
    std::vector<std::string> ss;
    for (auto& pd : percDone_) {
      if (pd.shouldShow()) {
        ss.push_back(msg(pd));
      }
    }
    return om::string::join(ss, " / ");
  }

 private:
  std::string msg(PercDone& pd) {
    if (pd.noMeshData()) {
      return "No meshes";
    }
    return om::string::num(pd.percentDone());
  }
};
}
}  // om::v3d::
