#pragma once

#include "common/common.h"
#include "boost/variant.hpp"
#include "enum/enum.hpp"

namespace om {
namespace volume {

template <typename T> class memMappedVolumeImpl;

class channel;
class segmentation;

typedef boost::variant<memMappedVolumeImpl<int8_t>,
                       memMappedVolumeImpl<uint8_t>,
                       memMappedVolumeImpl<int32_t>,
                       memMappedVolumeImpl<uint32_t>,
                       memMappedVolumeImpl<float> > volDataSrcs;

typedef boost::variant<int8_t*, uint8_t*, int32_t*, uint32_t*, float*> rawDataPtrs;

// char* used for serialization (don't change!)
BOOST_ENUM_VALUES(volDataType, std::string,
                  (UNKNOWN)("unknown")
                  (INT8)("int8_t")
                  (UINT8)("uint8_t")
                  (INT32)("int32_t")
                  (UINT32)("uint32_t")
                  (FLOAT)("float")
    )

class volumeTypeHelpers {
public:
    static std::string GetTypeAsString(const volDataType type);
    
    static volDataType GetTypeFromString(const std::string & type);

    static int getHDF5FileType(const volDataType type);
    static int getHDF5MemoryType(const volDataType type);
};

} // namespace volume
} // namespace om