#pragma once
#include "precomp.h"

namespace om {
namespace segment {

enum class coloring {
  FILTER_BLACK_BRIGHTEN_SELECT = 0, FILTER_COLOR_BRIGHTEN_SELECT, FILTER_BREAK,
  FILTER_VALID, FILTER_VALID_BLACK, SEGMENTATION, SEGMENTATION_BREAK_BLACK,
  SEGMENTATION_BREAK_COLOR, SEGMENTATION_VALID, SEGMENTATION_VALID_BLACK,
  FILTER_BLACK_DONT_BRIGHTEN_SELECT, FILTER_COLOR_DONT_BRIGHTEN_SELECT,
  NUMBER_OF_ENUMS
};
}
}  // om::segment::
