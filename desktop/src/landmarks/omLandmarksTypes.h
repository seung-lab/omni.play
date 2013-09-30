#pragma once

#include "utility/segmentDataWrapper.hpp"
#include "common/common.h"

namespace om {
namespace landmarks {

struct sdwAndPt {
  SegmentDataWrapper sdw;
  om::globalCoord coord;
};

std::string outputPt(const sdwAndPt& d);

}  // namespace landmarks
}  // namespace om
