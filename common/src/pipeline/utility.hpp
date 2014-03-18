#pragma once

#include "precomp.h"
#include "pipeline/stage.hpp"

namespace om {
namespace pipeline {

class write_out : public stage {
 private:
  std::string& dest_;

 public:
  write_out(std::string& dest) : dest_(dest) {}

  template <typename T>
  data_var operator()(const data<T>& d) const {
    int size = d.size * sizeof(T);
    dest_.resize(size);
    char* data = reinterpret_cast<char*>(d.data.get());
    dest_.assign(data, size);
    return d;
  }
};

}  // namespace pipeline
}  // namespace om
