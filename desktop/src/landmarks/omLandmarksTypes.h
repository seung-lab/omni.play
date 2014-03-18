#pragma once
#include "precomp.h"

#include "utility/segmentDataWrapper.hpp"
#include "common/common.h"

namespace om {
namespace landmarks {

struct sdwAndPt {
  SegmentDataWrapper sdw;
  om::coords::Global coord;
};

std::string outputPt(const sdwAndPt& d);

}  // namespace landmarks
}  // namespace om
