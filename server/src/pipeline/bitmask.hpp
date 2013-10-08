#pragma once

#include "pipeline/stage.hpp"
#include "utility/smartPtr.hpp"

namespace om {
namespace pipeline {

class bitmask : public stage {
 private:
  uint32_t mask_;

 public:
  bitmask(uint32_t mask) : mask_(mask) {}

  template <typename T> data_var operator()(const data<T>& in) const {
    data<bool> out;
    out.size = in.size;
    out.data = utility::smartPtr<bool>::MallocNumElements(out.size);
    for (int i = 0; i < out.size; i++) {
      out.data.get()[i] = in.data.get()[i] == mask_;
    }
    return out;
  }
};

}
}
