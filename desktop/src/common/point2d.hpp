#pragma once

namespace om {

template <typename T> struct point2d {
  T x;
  T y;

  inline bool operator<(const point2d<T>& b) const {
    if (x != b.x) {
      return x < b.x;
    }
    return y < b.y;
  }
};

typedef point2d<int> point2di;
typedef point2d<float> point2df;  // worry about comparison?

}  // om
