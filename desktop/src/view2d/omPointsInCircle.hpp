#pragma once

#include "common/om.hpp"
#include "zi/omMutex.h"

#include <map>
#include <vector>

class OmPointsInCircle {
 private:
  std::map<int, std::vector<om::point2di> > ptsInCircle_;

  zi::mutex lock_;

 public:
  const std::vector<om::point2di>& GetPtsInCircle(const int brushDia) {
    zi::guard g(lock_);

    if (!ptsInCircle_.count(brushDia)) {
      return ptsInCircle_[brushDia] = makePtrListInCircle(brushDia);
    }
    return ptsInCircle_[brushDia];
  }

 private:
  static std::vector<om::point2di> makePtrListInCircle(const int brushDia) {
    const int radius = brushDia / 2;
    const int sqRadius = radius * radius;

    std::vector<om::point2di> pts;
    pts.reserve(brushDia * brushDia);

    for (int i = 0; i < brushDia; ++i) {
      const int x = i - radius;

      for (int j = 0; j < brushDia; ++j) {
        const int y = j - radius;

        if (x * x + y * y <= sqRadius) {
          om::point2di p = { x, y };
          pts.push_back(p);

          // std::cout << "adding pt: " << x << ", " << y << "\n";
        }
      }
    }

    return pts;
  }
};
