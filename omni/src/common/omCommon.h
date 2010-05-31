#ifndef OM_COMMON_H
#define OM_COMMON_H

#define QT_USE_FAST_CONCATENATION
#define QT_USE_FAST_OPERATOR_PLUS

#include <QString>

#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/strong_typedef.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <boost/tr1/unordered_set.hpp>
#include <boost/utility.hpp>

#include "common/omStd.h"
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
static const OmId NULL_SEGMENT_ID = 0;

//internal storage size of segment data
#define SEGMENT_DATA_BYTES_PER_SAMPLE 4
#define SEGMENT_DATA_SAMPLES_PER_PIXEL 1

//BOOST_STRONG_TYPEDEF(quint32, OmSegID )
typedef quint32 OmSegID;
typedef boost::unordered_set<OmSegID> OmSegIDsSet;
typedef std::vector<OmSegID> OmSegIDsList;
typedef boost::shared_ptr<OmSegIDsList> OmSegIDsListPtr;

static const OmSegID NULL_SEGMENT_DATA(0);


/** 
 * System-state related
 **/
enum OmSlicePlane { SLICE_XY_PLANE, SLICE_XZ_PLANE, SLICE_YZ_PLANE };
enum OmSystemMode { NAVIGATION_SYSTEM_MODE, EDIT_SYSTEM_MODE, DEND_MODE };
enum OmToolMode { SELECT_MODE,
		  PAN_MODE,
		  CROSSHAIR_MODE,
		  ZOOM_MODE, 
		  ADD_VOXEL_MODE, 
		  SUBTRACT_VOXEL_MODE, 
		  SELECT_VOXEL_MODE,
		  FILL_MODE, 
		  VOXELIZE_MODE,
};


#endif
