#pragma once

#include "pipeline/stage.hpp"

namespace om {
namespace pipeline {

class bitmask : public stage {
 private:
  uint32_t mask_;

 public:
  bitmask(uint32_t mask) : mask_(mask) {}

  template <typename T> data_var operator()(const Data<T>& in) const {
    Data<bool> out(in.size);
    for (auto i = 0; i < out.size; i++) {
      out.data.get()[i] = in.data.get()[i] == mask_;
    }
    return out;
  }
};
}
}
