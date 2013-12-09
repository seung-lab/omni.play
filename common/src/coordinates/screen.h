#pragma once

#include "common/macro.hpp"
#include "vmmlib/vmmlib.h"

namespace om {
namespace coords {

class Global;
class ScreenSystem;

class Screen : public vmml::Vector2i {
 private:
  typedef vmml::Vector2i base_t;

 public:
  Screen(base_t v, const ScreenSystem& system) : base_t(v), system_(&system) {}

  Screen(int x, int y, const ScreenSystem& system)
      : base_t(x, y), system_(&system) {}

  Screen(const Screen& coord) : base_t(coord), system_(coord.system_) {}

  Global ToGlobal() const;

  const ScreenSystem& system() const { return *system_; }

 private:
  const ScreenSystem* system_;
};

}  // namespace coords
}  // namespace om
