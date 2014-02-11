#include "coordinates/data.h"
#include "coordinates/global.h"
#include "common/common.h"
#include "utility/yaml/omBaseTypes.hpp"

namespace YAMLold {

Emitter& operator<<(Emitter& e, const om::coords::Global& c) {
  e << (Vector3f)c;
  return e;
}

void operator>>(const Node& n, om::coords::Global& c) {
  Vector3f& vec = c;
  n >> vec;
}

Emitter& operator<<(Emitter& e, const om::coords::GlobalBbox& box) {
  e << (AxisAlignedBoundingBox<float>)box;
  return e;
}

void operator>>(const Node& n, om::coords::GlobalBbox& box) {
  AxisAlignedBoundingBox<float>& bbox = box;
  n >> bbox;
}

}  // namespace YAMLold
