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

#endif
