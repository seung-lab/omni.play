#ifndef OM_VOLUME_TYPES_HPP
#define OM_VOLUME_TYPES_HPP

#include "common/omCommon.h"
#include "boost/variant.hpp"
#include "enum/enum.hpp"

template <typename T> class OmMemMappedVolumeImpl;

class OmChannel;
class OmSegmentation;

typedef	boost::variant<OmMemMappedVolumeImpl<int8_t>,
					   OmMemMappedVolumeImpl<uint8_t>,
					   OmMemMappedVolumeImpl<int32_t>,
					   OmMemMappedVolumeImpl<uint32_t>,
					   OmMemMappedVolumeImpl<float> >
OmVolDataSrcs;

typedef boost::variant<int8_t*, uint8_t*, int32_t*, uint32_t*, float*>
OmRawDataPtrs;

// char* used for serialization (don't change!)
BOOST_ENUM_VALUES(OmVolDataType, std::string,
    (UNKNOWN)("unknown")
    (INT8)("int8_t")
    (UINT8)("uint8_t")
    (INT32)("int32_t")
    (UINT32)("uint32_t")
    (FLOAT)("float")
)

class OmVolumeTypeHelpers {
public:
	static std::string GetTypeAsString(const OmVolDataType type);
	static QString GetTypeAsQString(const OmVolDataType type);

	static OmVolDataType GetTypeFromString(const QString & type);

	static int getHDF5FileType(const OmVolDataType type);
	static int getHDF5MemoryType(const OmVolDataType type);
};

#endif
