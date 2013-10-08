#include "common/omCommon.h"
#include "common/omString.hpp"
#include "zi/omUtility.h"

std::ostream& operator<<(std::ostream& out, const OmSegIDsSet& in) {
  const std::string joined = om::string::join(in);

  out << "[" << joined << "]";
  return out;
}

std::ostream& operator<<(std::ostream& out, const om::tool::mode& tool) {
  switch (tool) {
    case om::tool::SELECT:
      out << "SELECT";
      break;
    case om::tool::PAN:
      out << "PAN";
      break;
    case om::tool::PAINT:
      out << "PAINT";
      break;
    case om::tool::ERASE:
      out << "ERASE";
      break;
    case om::tool::FILL:
      out << "FILL";
      break;
    case om::tool::SPLIT:
      out << "SPLIT";
      break;
    case om::tool::SHATTER:
      out << "SHATTER";
      break;
    case om::tool::CUT:
      out << "CUT";
      break;
    default:
      out << "unknown tool";
      break;
  }
  ;

  return out;
}

std::ostream& operator<<(std::ostream& out, const om::CacheGroup& c) {
  if (om::MESH_CACHE == c) {
    out << "MESH_CACHE";
  } else {
    out << "TILE_CACHE";
  }
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
      throw OmArgException("unknown viewtype");
  }

  return out;
}
