#pragma once

#include "common/omQt.h"
#include "common/omBoost.h"

#include "common/omStd.h"
#include "common/omException.h"
#include <cassert>

#include "common/omMath.hpp"

#include "utility/omSharedPtr.hpp"
#include <zi/for_each.hpp>



/**
 * "system" types
 */
//id typedefs
typedef uint32_t OmID;
typedef boost::unordered_set< OmID > OmIDsSet;

//bit field
typedef unsigned int OmBitfield;

//slices
enum ViewType { XY_VIEW, XZ_VIEW, ZY_VIEW };
std::ostream& operator<<(std::ostream &out, const ViewType& vt);

//objects
enum ObjectType { CHANNEL, SEGMENTATION };

// Must be here so that ViewType is already defined.
#include "coordinates/coordinates.h"

/**
 * "segment" types
 */
typedef uint32_t OmSegID;
typedef std::deque<OmSegID> OmSegIDsList;
typedef uint32_t PageNum;

class OmSegment;
typedef std::set<OmSegment*> OmSegPtrSet;
typedef std::set<OmSegID> OmSegIDsSet;
std::ostream& operator<<(std::ostream& out, const OmSegIDsSet& in);


/**
 * "group" types
 */
typedef uint32_t OmGroupID;
typedef QString OmGroupName;
typedef boost::unordered_set<OmGroupID> OmGroupIDsSet;


/**
 * System-state related
 **/
namespace om {
namespace tool {

enum mode { SELECT,
            PAN,
            CROSSHAIR,
            ZOOM,
            PAINT,
            ERASE,
            FILL,
            SPLIT,
            CUT,
            LANDMARK,
            ANNOTATE
};

} //namespace tool
} //namespace om

std::ostream& operator<<(std::ostream &out, const om::tool::mode& c);


/**
 * color cache enum
 */
enum OmSegmentColorCacheType{SCC_FILTER_BLACK_BRIGHTEN_SELECT = 0,
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
namespace om {
enum CacheGroup {
    MESH_CACHE = 1,
    TILE_CACHE
};

} // om
std::ostream& operator<<(std::ostream &out, const om::CacheGroup& c);

