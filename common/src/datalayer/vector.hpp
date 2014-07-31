#pragma once

#include "precomp.h"

namespace om {
namespace datalayer {

template <typename TKey, typename TValue>
struct Vector {
  Vector(const TKey& key) : Key(key) {}
  template <typename InputIterator>
  Vector(const TKey& key, InputIterator from, InputIterator to)
      : Key(key), Values(from, to) {}

  TKey Key;
  std::vector<TValue> Values;

  typedef typename std::vector<TValue>::iterator iterator;
  typedef typename std::vector<TValue>::const_iterator const_iterator;
  iterator begin() { return Values.begin(); }
  iterator end() { return Values.end(); }
  const_iterator begin() const { return Values.begin(); }
  const_iterator end() const { return Values.end(); }

  decltype(Values.size()) size() { return Values.size(); }

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
