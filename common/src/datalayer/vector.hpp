#pragma once

#include "precomp.h"

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

  template <typename ForwardIterator>
  bool contains_any(ForwardIterator begin, ForwardIterator end) const {
    for (; begin != end; ++begin) {
      if (contains(*begin)) {
        return true;
      }
    }
    return false;
  }
};
}
}  // namespace om::datalayer::
