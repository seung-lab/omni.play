#pragma once

#include <zi/parallel/algorithm.hpp>

namespace om {
namespace set {

template <typename T>
inline bool SetAContainsB(const std::set<T>& a, const std::set<T>& b) {
  std::set<T> diff;

  zi::set_difference(b.begin(), b.end(), a.begin(), a.end(),
                     std::inserter(diff, diff.begin()));

  return 0 == diff.size();
}

template <typename T>
inline bool SetsAreDisjoint(const std::set<T>& a, const std::set<T>& b) {
  std::set<T> inter;

  zi::set_intersection(b.begin(), b.end(), a.begin(), a.end(),
                       std::inserter(inter, inter.begin()));

  return 0 == inter.size();
}

template <typename T>
inline void merge(const std::set<T>& a, const std::set<T>& b, std::set<T>& c) {
  zi::set_union(a.begin(), a.end(), b.begin(), b.end(),
                std::inserter(c, c.begin()));
}

template <typename T>
inline void mergeBintoA(std::set<T>& a, const std::set<T>& b) {
  for (auto& c : b) {
    a.insert(c);
  }
}

template <typename T>
inline void mergeBintoA(std::unordered_set<T>& a,
                        const std::unordered_set<T>& b) {
  for (auto& c : b) {
    a.insert(c);
  }
}

}  // set
}  // om
