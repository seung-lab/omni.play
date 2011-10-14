#pragma once

#include "boost.h"

#include "std.h"
#include "common/exception.h"
#include <cassert>

#include "math.hpp"

// needed for coordinates
namespace om { namespace common {
    enum viewType { XY_VIEW, XZ_VIEW, ZY_VIEW };
    std::ostream& operator<<(std::ostream &out, const viewType& vt);
}}

#include "coordinates/coordinates.h"
#include <zi/for_each.hpp>

namespace om {
namespace common {

/**
 * "system" types
 */
//id typedefs
typedef uint32_t id;
typedef boost::unordered_set< id > idSet;

//bit field
typedef unsigned int bitfield;

//objects
//slices
enum objectType { CHANNEL, SEGMENTATION };

/**
 * "segment" types
 */
typedef uint32_t segID;
typedef std::deque<segID> segIdList;
typedef uint32_t pageNum;

class segment;
typedef std::set<segment*> segPtrSet;
typedef std::set<segID> segIdSet;
std::ostream& operator<<(std::ostream& out, const segIdSet& in);

/**
 * "group" types
 */
typedef uint32_t groupId;
typedef std::string groupName;
typedef boost::unordered_set<groupId> groupIdSet;

/**
 * color cache enum
 */
enum segmentColorCacheType { SCC_FILTER_BLACK_BRIGHTEN_SELECT = 0,
                             SCC_FILTER_COLOR_BRIGHTEN_SELECT,
                             SCC_FILTER_BREAK,
                             SCC_FILTER_VALID,
                             SCC_FILTER_VALID_BLACK,
                             SCC_SEGMENTATION,
                             SCC_SEGMENTATION_BREAK_BLACK,
                             SCC_SEGMENTATION_BREAK_COLOR,
                             SCC_SEGMENTATION_VALID,
                             SCC_SEGMENTATION_VALID_BLACK,
                             SCC_FILTER_BLACK_DONT_BRIGHTEN_SELECT,
                             SCC_FILTER_COLOR_DONT_BRIGHTEN_SELECT,
                             SCC_NUMBER_OF_ENUMS };

/**
 * cache-type enum
 */
enum cacheGroup {
    MESH_CACHE = 1,
    TILE_CACHE
};

std::ostream& operator<<(std::ostream &out, const cacheGroup& c);

} // common
} // om
