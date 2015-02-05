#include "precomp.h"
#include "common/common.h"
#include "string.hpp"
#include "zi/utility.h"

namespace om {
namespace common {

std::ostream& operator<<(std::ostream& out, const SegIDSet& in) {
  const std::string joined = om::string::join(in);

  out << "[" << joined << "]";
  return out;
}

std::ostream& operator<<(std::ostream& out, const ViewType& vt) {
  switch (vt) {
    case XY_VIEW:
      out << "XY_VIEW";
      break;
    case XZ_VIEW:
      out << "XZ_VIEW";
      break;
    case ZY_VIEW:
      out << "ZY_VIEW";
      break;
    default:
      throw ArgException("unknown viewtype");
  }

  return out;
}

}  // namespace common
}  // namespace om

namespace std {
template class std::unordered_set<om::common::ID>;

template class std::deque<om::common::SegID>;
template class std::set<om::common::SegID>;
}

namespace {
// Even though we have explicitly instantiated
// std::unordered_set<om::common::ID>, for some reason g++ (4.8) refuses
// to generate code for its copy and move construstors and operator=
// (it wasn't becuase we extern'ed the class earlier in the header),
// causing linking errors in a build with all optimization disabled.
// Force the code to be generated:
void NotUsed() {
  std::unordered_set<om::common::ID> a(a);
  std::unordered_set<om::common::ID> b(std::move(a));
  a = a;
  a = std::move(a);
}
}