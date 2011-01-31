#ifndef OM_COMMON_H
#define OM_COMMON_H

#define BOOST_MULTI_INDEX_DISABLE_SERIALIZATION

#define QT_USE_FAST_CONCATENATION
#define QT_USE_FAST_OPERATOR_PLUS

#include <QString>

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/strong_typedef.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <boost/tr1/unordered_set.hpp>
#include <boost/utility.hpp>

#include "common/omStd.h"
#include "common/omException.h"
#include <cassert>

class QTextStream;

/**
 * color structs
 *
 **/
struct OmColor {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};
std::ostream& operator<<(std::ostream &out, const OmColor& c);
bool operator<(const OmColor& a, const OmColor& b);
QTextStream &operator<<(QTextStream& out, const OmColor& c);

struct OmColorRGBA {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
};
std::ostream& operator<<(std::ostream &out, const OmColorRGBA& c);


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
typedef uint32_t OmID;
typedef boost::unordered_set< OmID > OmIDsSet;

//bit field
typedef unsigned int OmBitfield;

//slices
enum ViewType { XY_VIEW, XZ_VIEW, YZ_VIEW };

//objects
enum ObjectType { CHANNEL, SEGMENTATION };


/**
 * "segment" types
 */
typedef uint32_t OmSegID;
typedef std::set<OmSegID> OmSegIDsSet;
typedef std::deque<OmSegID> OmSegIDsList;
typedef uint32_t PageNum;


/**
 * GUI segment list types
 */
namespace om {
	// WARNING: used for serialization by OmSegmentPage
	enum OmSegListType {WORKING = 0,
						VALID = 1,
						UNCERTAIN = 2};
};


/**
 * "group" types
 */
typedef uint32_t OmGroupID;
typedef QString OmGroupName;
typedef boost::unordered_set<OmGroupID> OmGroupIDsSet;


/**
 * System-state related
 **/
enum OmToolMode { SELECT_MODE,
				  PAN_MODE,
				  CROSSHAIR_MODE,
				  ZOOM_MODE,
				  ADD_VOXEL_MODE,
				  SUBTRACT_VOXEL_MODE,
				  FILL_MODE,
				  SPLIT_MODE,
				  CUT_MODE
};


/**
 * color cache enum
 */
enum OmSegmentColorCacheType{SCC_FILTER_BLACK = 0,
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

class OmTile;
typedef boost::shared_ptr<OmTile> OmTilePtr;

/**
 * string-to-number and number-to-string converters
 **/
namespace om {
	template <typename T>
	static std::string NumToStr(const T& num){
		return boost::lexical_cast<std::string>(num);
	}

	template <typename T>
	static T StrToNum(const std::string& str){
		return boost::lexical_cast<T>(str);
	}

	template <typename T>
	static T StrToNum(const QString& str){
		return boost::lexical_cast<T>(str.toStdString());
	}
}

#endif
