#pragma once

#include <cstring>
#include "enum/enum.hpp"

namespace om {
namespace common {

enum class ZeroMem {
  ZERO_FILL, DONT_ZERO_FILL
};
enum class Blocking {
  BLOCKING, NON_BLOCKING
};
enum class SetValid {
  SET_VALID, SET_NOT_VALID
};
enum class AffinityGraph {
  NO_AFFINITY, X_AFFINITY, Y_AFFINITY, Z_AFFINITY
};
enum class ShouldBufferWrites {
  BUFFER_WRITES, DONT_BUFFER_WRITES
};
enum class AllowOverwrite {
  WRITE_ONCE, OVERWRITE
};
enum class AddOrSubtract {
  ADD, SUBTRACT
};
enum class Direction {
  LEFT, RIGHT, UP, DOWN
};

enum class SegListType : uint8_t {
  WORKING = 0, VALID = 1, UNCERTAIN = 2
};

enum class CacheGroup {
  MESH_CACHE = 1, TILE_CACHE
};

inline std::ostream& operator<<(std::ostream& out, const CacheGroup& c) {
  if (CacheGroup::MESH_CACHE == c) {
    out << "MESH_CACHE";
  } else {
    out << "TILE_CACHE";
  }
  return out;
}

BOOST_ENUM_VALUES(DataType, std::string,
                  (UNKNOWN)("unknown")(INT8)("int8_t")(UINT8)("uint8_t")(INT32)(
                      "int32_t")(UINT32)("uint32_t")(FLOAT)("float"))

}  // common
}  // om
