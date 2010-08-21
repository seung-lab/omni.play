#ifndef OM_VOLUME_TYPES_HPP
#define OM_VOLUME_TYPES_HPP

#include "common/omCommon.h"
#include "boost/variant.hpp"
#include "enum/enum.hpp"

typedef boost::variant<int8_t*, uint8_t*, int32_t*, uint32_t*, float*>
OmRawDataPtrs;

typedef boost::unordered_map<OmSegID, uint32_t>
OmSegSizeMap;

typedef boost::shared_ptr<OmSegSizeMap>
OmSegSizeMapPtr;

typedef boost::unordered_map<OmSegID, DataBbox>
OmSegBounds;

// char* used for serialization (don't change!)
BOOST_ENUM_VALUES(OmVolDataType, const char*,
    (UNKNOWN)("unknown")
    (OM_INT8)("int8_t")
    (OM_UINT8)("uint8_t")
    (OM_INT32)("int32_t")
    (OM_UINT32)("uint32_t")
    (OM_FLOAT)("float")
)

class OmVolumeTypeHelpers {
public:
	static std::string GetTypeAsString(const OmVolDataType type){
		return std::string(type.value());
	}

	// used for serialization (don't change!)
	static OmVolDataType GetTypeFromString(const QString & type){
		boost::optional<OmVolDataType> ret =
			OmVolDataType::get_by_value(qPrintable(type));
		if(!ret){
			throw OmIoException("invalid type");
		}
		return *ret;
	}
};


#endif
