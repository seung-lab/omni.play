#pragma once
#include "precomp.h"

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
  task,
  connection,
  NUM_TOTAL
};
}
}  // om::event::
