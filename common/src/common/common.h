#pragma once

#include "boost.h"

#include "std.h"
#include "common/exception.h"
#include <cassert>

#include "math.hpp"

// needed for coordinates
namespace om { namespace common {
    enum ViewType { XY_VIEW, XZ_VIEW, ZY_VIEW };
    std::ostream& operator<<(std::ostream &out, const ViewType& vt);
}}

#include "coordinates/coordinates.h"
#include <zi/for_each.hpp>

namespace om {
namespace common {

/**
 * "system" types
 */
//id typedefs
typedef uint32_t ID;
typedef boost::unordered_set< ID > IDSet;

//bit field
typedef unsigned int Bitfield;

//objects
//slices
enum ObjectType { CHANNEL, SEGMENTATION };

/**
 * "segment" types
 */
typedef uint32_t SegID;
typedef std::deque<SegID> SegIDList;
typedef std::set<SegID> SegIDSet;
std::ostream& operator<<(std::ostream& out, const SegIDSet& in);

typedef uint32_t PageNum;
/**
 * "group" types
 */
typedef uint32_t GroupID;
typedef std::string GroupIDoupName;
typedef boost::unordered_set<GroupID> GroupIDSet;

template<typename T>
T twist(T vec, ViewType view)
{
    T out(vec);
    switch(view)
    {
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

} // common
} // om


