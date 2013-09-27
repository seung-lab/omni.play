#pragma once

#include "pipeline/stage.hpp"

namespace om {
namespace pipeline {

class mask : public stage {
 private:
  uint32_t mask_;

 public:
  mask(uint32_t mask) : mask_(mask) {}

  template <typename T>
  data_var operator()(const Data<T>& in) const {
    Data<T> out(in.size);
    for (auto i = 0; i < in.size; ++i) {
      out.data.get()[i] = (T)(mask_ | ((uint32_t)in.data.get()[i]));
    }

    return out;
  }
};
}
}  // namespace om::pipeline::