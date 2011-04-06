#ifndef OM_COMMON_H
#define OM_COMMON_H

#include "common/omQt.h"
#include "common/omBoost.h"

#include "common/omStd.h"
#include "common/omException.h"
#include <cassert>

/**
 * vmml-related
 */
#include <vmmlib/vmmlib.h>
using namespace vmml;

//coordinate frames
typedef vmml::Vector3<int> DataCoord;
typedef vmml::Vector3<float> NormCoord;
typedef vmml::Vector2<int> ScreenCoord;

typedef vmml::AxisAlignedBoundingBox<int> DataBbox;
typedef vmml::AxisAlignedBoundingBox<float> NormBbox;
typedef vmml::AxisAlignedBoundingBox<float> SpaceBbox;


/**
 * "system" types
 */
//id typedefs
typedef uint32_t OmID;
typedef boost::unordered_set< OmID > OmIDsSet;

//bit field
typedef unsigned int OmBitfield;

//slices
enum ViewType { XY_VIEW, XZ_VIEW, YZ_VIEW };
std::ostream& operator<<(std::ostream &out, const ViewType& vt);

//objects
enum ObjectType { CHANNEL, SEGMENTATION };


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
            CUT
};

} //namespace tool
} //namespace om

std::ostream& operator<<(std::ostream &out, const om::tool::mode& c);


/**
 * color cache enum
 */
enum OmSegmentColorCacheType{SCC_FILTER_BLACK = 0,
                             SCC_FILTER_COLOR,
                             SCC_FILTER_BREAK,
                             SCC_FILTER_VALID,
                             SCC_FILTER_VALID_BLACK,
                             SCC_SEGMENTATION,
                             SCC_SEGMENTATION_BREAK_BLACK,
                             SCC_SEGMENTATION_BREAK_COLOR,
                             SCC_SEGMENTATION_VALID,
                             SCC_SEGMENTATION_VALID_BLACK,
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

class OmTile;
typedef boost::shared_ptr<OmTile> OmTilePtr;

#endif
