#pragma once

namespace om {
namespace fuzzymap_ {
struct CompareDoubles {
  bool operator()(const double a, const double b) const {
    if (qFuzzyCompare(a, b)) {
      return false;
    }
    return a < b;
  }
};
}  // namespace fuzzymap_
}  // namespace om

template <typename T>
class DoubleFuzzyStdMap
    : public std::map<double, T, om::fuzzymap_::CompareDoubles> {
};

class DoubleFuzzyStdSet
    : public std::set<double, om::fuzzymap_::CompareDoubles> {
};
