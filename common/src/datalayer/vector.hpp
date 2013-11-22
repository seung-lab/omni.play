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
    // Assumes sorted
    return std::binary_search(Values.begin(), Values.end(), value);
  }
};
}
}  // namespace om::datalayer::
