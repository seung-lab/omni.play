#pragma once

#include "core.hpp"
#include "logging.h"

#include "common/exception.h"
#include <cassert>
#include <unordered_set>
#include <deque>
#include <set>
#include <iostream>

#include "math.hpp"

// needed for coordinates
#include <vmmlib/vmmlib.h>
using namespace vmml;

// C++14 make_unique, ala http://stackoverflow.com/q/7038357
namespace std {
template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&& ... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
};

namespace om {
namespace common {

enum ViewType {
  XY_VIEW,
  XZ_VIEW,
  ZY_VIEW
};
std::ostream& operator<<(std::ostream& out, const ViewType& vt);

/**
 * "system" types
 */
// id typedefs
typedef std::unordered_set<ID> IDSet;

/**
 * "segment" types
 */
typedef std::deque<SegID> SegIDList;
typedef std::set<SegID> SegIDSet;
std::ostream& operator<<(std::ostream& out, const SegIDSet& in);

typedef uint32_t PageNum;

template <typename T> T twist(T vec, ViewType view) {
  T out(vec);
  switch (view) {
    case XY_VIEW:
      break;
    case XZ_VIEW:
      out.x = vec.x;
      out.y = vec.z;
      out.z = vec.y;
      break;
    case ZY_VIEW:
      out.x = vec.z;
      out.y = vec.y;
      out.z = vec.x;
      break;
  }

  return out;
}

}  // common
}  // om
