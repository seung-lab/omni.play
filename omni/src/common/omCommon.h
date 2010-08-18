#ifndef OM_COMMON_H
#define OM_COMMON_H

#define QT_USE_FAST_CONCATENATION
#define QT_USE_FAST_OPERATOR_PLUS

#include <QString>

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/strong_typedef.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <boost/tr1/unordered_set.hpp>
#include <boost/utility.hpp>

#include "common/omStd.h"
#include "common/omException.h"
#include <assert.h>

// elimiate VTK warning
#define VTK_EXCLUDE_STRSTREAM_HEADERS

typedef struct {
	quint8 red;
	quint8 green;
	quint8 blue;
} OmColor;



/**
 * vmml-related
 */
#include <vmmlib/vmmlib.h>
using namespace vmml;

//coordinate frames
typedef vmml::Vector3<int> DataCoord;
typedef vmml::Vector3<float> NormCoord;
typedef vmml::Vector3<float> SpaceCoord;
typedef vmml::Vector2<int> ScreenCoord;

typedef vmml::AxisAlignedBoundingBox<int> DataBbox;
typedef vmml::AxisAlignedBoundingBox<float> NormBbox;
typedef vmml::AxisAlignedBoundingBox<float> SpaceBbox;



/**
 * "system" types
 */
//id typedefs
typedef quint32 OmId;
typedef boost::unordered_set< OmId > OmIDsSet;

//bit field
typedef unsigned int OmBitfield;

//slices
enum ViewType { XY_VIEW, XZ_VIEW, YZ_VIEW };

//objects
enum ObjectType { VOLUME, CHANNEL, SEGMENTATION, SEGMENT, NOTE, FILTER };


/**
 * "segment" types
 */
//internal storage size of segment data
#define SEGMENT_DATA_SAMPLES_PER_PIXEL 1

typedef uint32_t OmSegID;
static const OmSegID NULL_SEGMENT_VALUE(0);
typedef boost::unordered_set<OmSegID> OmSegIDsSet;
typedef std::vector<OmSegID> OmSegIDsList;


/**
 * "group" types
 */
typedef quint32 OmGroupID;
enum OmSegIDRootType {NOTVALIDROOT, VALIDROOT, RECENTROOT, GROUPROOT};
typedef QString OmGroupName;
typedef boost::unordered_set<OmGroupID> OmGroupIDsSet;


/**
 * System-state related
 **/
enum OmSlicePlane { SLICE_XY_PLANE, SLICE_XZ_PLANE, SLICE_YZ_PLANE };
enum OmToolMode { SELECT_MODE,
		  PAN_MODE,
		  CROSSHAIR_MODE,
		  ZOOM_MODE,
		  ADD_VOXEL_MODE,
		  SUBTRACT_VOXEL_MODE,
		  SELECT_VOXEL_MODE,
		  FILL_MODE,
		  SPLIT_MODE
};


/**
 * color cache enum
 */
enum OmSegmentColorCacheType { SCC_FILTER_BLACK = 0,
			       SCC_FILTER_COLOR,
			       SCC_FILTER_BREAK,
			       SCC_FILTER_VALID,
			       SCC_FILTER_VALID_BLACK,
			       SCC_SEGMENTATION,
			       SCC_SEGMENTATION_BREAK,
			       SCC_SEGMENTATION_VALID,
			       SCC_SEGMENTATION_VALID_BLACK,
			       SCC_NUMBER_OF_ENUMS };

/**
 * cache-type enum
 */
enum OmCacheGroupEnum {
	RAM_CACHE_GROUP = 1,
	VRAM_CACHE_GROUP
};

enum OmAllowedVolumeDataTypes {
	UNKNOWN = 0,
	OM_INT8,
	OM_UINT8,
	OM_INT32,
	OM_UINT32,
	OM_FLOAT
};

#endif
