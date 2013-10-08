#pragma once

#include "vmmlib/vmmlib.h"

namespace om {
namespace coords {

class global;
class screenSystem;

class screen : public vmml::Vector2i {
 private:
  typedef vmml::Vector2i base_t;
  const screenSystem* system_;

 public:
  screen(base_t v, const screenSystem* system) : base_t(v), system_(system) {}

  screen(int x, int y, const screenSystem* system)
      : base_t(x, y), system_(system) {}

  screen(const screen& coord) : base_t(coord), system_(coord.system_) {}

  global toGlobal() const;
};

}  // namespace coords
}  // namespace om
