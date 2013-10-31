#pragma once

#include "utility/malloc.hpp"

namespace om {
namespace tile {

// TODO: this could be converted into a tile pool
//  (shared_ptr can take custom destructor....)

template <class T> std::shared_ptr<T> Make() {
  return om::mem::Malloc<T>::NumElementsDontZero(128 * 128);
}

}
}  // namespace
