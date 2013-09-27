#pragma once

#include <vector>

namespace om {
namespace datalayer {

template <typename TKey, typename TValue>
struct Vector {
  Vector(const TKey& key) : Key(key) {}

  TKey Key;
  std::vector<TValue> Values;

  bool contains(const TValue& value) const {
    // lower bound?
    return std::find(Values.begin(), Values.end(), value) != Values.end();
  }
};
}
}  // namespace om::datalayer::
