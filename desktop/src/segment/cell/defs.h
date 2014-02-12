#pragma once

#include "common/core.hpp"

typedef uint32_t segid;

#include <memory>

struct edge {
  segid child;
  segid parent;
  float edgeWeight;
};
