#pragma once

#include "segment/lowLevel/DynamicForestPool.hpp"

extern template class std::vector<float>;
extern template class std::vector<segid>;

namespace om {
namespace cell {

class Trees {
  std::vector<segid> parent_;  // 0 if no parent
  std::vector<float> edgeWeight_;
  zi::DynamicForestPool<segid> df_;

 public:
  Trees(uint32_t size) : parent_(size, 0), edgeWeight_(size, 0), df_(size) {}

  void Resize(uint32_t size) {
    df_.Resize(size);
    parent_.resize(size, 0);
    edgeWeight_.resize(size, 0);
  }

  inline segid Root(segid id) {
    if (!parent_[id]) {
      return id;  // tree root (could have 0 or more children)
    }
    return df_.Root(id);
  }

  inline segid DFRoot(segid id) { return df_.Root(id); }

  inline edge Split(segid id) {
    df_.Cut(id);
    edge e { id, parent_[id], edgeWeight_[id] }
    ;
    parent_[id] = 0;
    edgeWeight_[id] = 0;
    return e;
  }

  inline void Join(segid p, segid c, float edgeWeight) {
    df_.Join(c, p);
    parent_[c] = p;
    edgeWeight_[c] = edgeWeight;
  }
};
}
}  // om::cell::
