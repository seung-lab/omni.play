#pragma once

#include "vmmlib/vmmlib.h"

class OmView2dState;

namespace om {

class globalCoord;

class screenCoord : public vmml::Vector2i {
 private:
  typedef vmml::Vector2i base_t;
  const OmView2dState* state_;

 public:
  screenCoord(base_t v, const OmView2dState* state)
      : base_t(v), state_(state) {}

  screenCoord(int x, int y, const OmView2dState* state)
      : base_t(x, y), state_(state) {}

  screenCoord(const screenCoord& coord) : base_t(coord), state_(coord.state_) {}

  globalCoord toGlobalCoord() const;
};

}  // namespace om