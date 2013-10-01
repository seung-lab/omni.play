#pragma once

#include <string>
#include "common/exception.h"

namespace om {
namespace event {

enum Klass {
  annotation = 0,
  nonFatalEvent,
  preference,
  mst,
  segment,
  tool,
  ui,
  view2d,
  view3d,
  NUM_TOTAL
};
}
}  // om::event::
