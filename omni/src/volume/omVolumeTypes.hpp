#ifndef OM_VOLUME_TYPES_HPP
#define OM_VOLUME_TYPES_HPP

#include "common/omCommon.h"
#include "boost/variant.hpp"

typedef boost::variant<int8_t*, uint8_t*, int32_t*, uint32_t*, float*>
OmRawDataPtrs;

typedef boost::unordered_map<OmSegID, uint32_t>
OmSegSizeMap;

typedef boost::shared_ptr<OmSegSizeMap>
OmSegSizeMapPtr;

typedef boost::unordered_map<OmSegID, DataBbox>
OmSegBounds;

enum OmAllowedVolumeDataTypes {
	UNKNOWN = 0,
	OM_INT8,
	OM_UINT8,
	OM_INT32,
	OM_UINT32,
	OM_FLOAT
};

class OmVolumeTypeHelpers {
public:
	static std::string GetTypeAsString(const OmAllowedVolumeDataTypes type){
		switch(type){
		case OM_INT8:
			return "int8_t";
		case OM_UINT8:
			return "uint8_t";
		case OM_INT32:
			return "int32_t";
		case OM_UINT32:
			return "uint32_t";
		case OM_FLOAT:
			return "float";
		case UNKNOWN:
			return "unkown";
		default:
			return "invalid";
		}
	}
};


#endif
