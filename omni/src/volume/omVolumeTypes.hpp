#ifndef OM_VOLUME_TYPES_HPP
#define OM_VOLUME_TYPES_HPP

#include "common/omCommon.h"
#include "boost/variant.hpp"
#include "enum/enum.hpp"

class OmMipChunk;
typedef boost::shared_ptr<OmMipChunk> OmMipChunkPtr;

typedef boost::unordered_map<OmSegID, uint32_t>
OmSegSizeMap;

template <typename T, typename VOL> class OmMemMappedVolume;
template <typename T, typename VOL> class OmHDF5Volume;
class OmChannel;
class OmSegmentation;

typedef	boost::variant<OmMemMappedVolume<int8_t, OmChannel>,
		       OmMemMappedVolume<uint8_t, OmChannel>,
		       OmMemMappedVolume<int32_t, OmChannel>,
		       OmMemMappedVolume<uint32_t, OmChannel>,
		       OmMemMappedVolume<float, OmChannel>,
		       OmMemMappedVolume<int8_t, OmSegmentation>,
		       OmMemMappedVolume<uint8_t, OmSegmentation>,
		       OmMemMappedVolume<int32_t, OmSegmentation>,
		       OmMemMappedVolume<uint32_t, OmSegmentation>,
		       OmMemMappedVolume<float, OmSegmentation>,
		       OmHDF5Volume<int8_t, OmChannel>,
		       OmHDF5Volume<uint8_t, OmChannel>,
		       OmHDF5Volume<int32_t, OmChannel>,
		       OmHDF5Volume<uint32_t, OmChannel>,
		       OmHDF5Volume<float, OmChannel>,
		       OmHDF5Volume<int8_t, OmSegmentation>,
		       OmHDF5Volume<uint8_t, OmSegmentation>,
		       OmHDF5Volume<int32_t, OmSegmentation>,
		       OmHDF5Volume<uint32_t, OmSegmentation>,
		       OmHDF5Volume<float, OmSegmentation> >
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
	static OmVolDataType GetTypeFromString(const QString & type);

	static int getHDF5FileType(const OmVolDataType type);
	static int getHDF5MemoryType(const OmVolDataType type);
};


#endif
