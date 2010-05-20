#ifndef OM_COMMON_H
#define OM_COMMON_H

#define QT_USE_FAST_CONCATENATION
#define QT_USE_FAST_OPERATOR_PLUS

#include <QString>
#include <QSet>

#include "common/omStd.h"
#include <assert.h>

// elimiate VTK warning
#define VTK_EXCLUDE_STRSTREAM_HEADERS

typedef struct {
	quint8 red;
	quint8 green;
	quint8 blue;
} OmColor;

#include "boost/strong_typedef.hpp"
BOOST_STRONG_TYPEDEF(quint32, OmSegID )

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
typedef QSet< OmId > OmIds;

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

typedef unsigned int SEGMENT_DATA_TYPE;
typedef QSet< SEGMENT_DATA_TYPE > SegmentDataSet;

static const SEGMENT_DATA_TYPE NULL_SEGMENT_DATA = 0;


#endif
