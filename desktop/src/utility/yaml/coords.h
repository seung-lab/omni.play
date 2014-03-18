#pragma once
#include "precomp.h"

namespace om {
namespace coords {
class Global;
class GlobalBbox;
}
}

namespace YAMLold {

Emitter& operator<<(Emitter&, const om::coords::Global&);
void operator>>(const Node&, om::coords::Global&);
Emitter& operator<<(Emitter&, const om::coords::GlobalBbox&);
void operator>>(const Node&, om::coords::GlobalBbox&);

}  // namespace YAMLold
