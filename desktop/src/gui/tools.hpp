#pragma once
#include "precomp.h"

namespace om {
namespace tool {

enum mode {
  SELECT,
  PAN,
  PAINT,
  ERASE,
  FILL,
  SPLIT,
  JOIN,
  CUT,
  LANDMARK,
  ANNOTATE,
  ADVANCED,
  SHATTER
};

std::string ToolToString(om::tool::mode tool);

}  // namespace tool
}  // namespace om
