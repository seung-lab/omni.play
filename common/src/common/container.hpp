#pragma once

#include "precomp.h"

namespace om {
namespace container {

using zi::containers::clear;

template <typename T>
void eraseRemove(std::vector<T>& v, const T val) {
  v.erase(std::remove(v.begin(), v.end(), val), v.end());
}

template <typename U>
void clearPtrVec(std::vector<U*>& vec) {
  for (auto* u : vec) {
    delete u;
  }
}

}  // namespace container
}  // namespace om
