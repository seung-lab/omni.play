#pragma once

#include "common/common.h"
#include "boost/variant.hpp"
#include "enum/enum.hpp"

template <typename T> class OmMemMappedVolumeImpl;

class OmChannel;
class OmSegmentation;

typedef boost::variant<OmMemMappedVolumeImpl<int8_t>,
                       OmMemMappedVolumeImpl<uint8_t>,
                       OmMemMappedVolumeImpl<int32_t>,
                       OmMemMappedVolumeImpl<uint32_t>,
                       OmMemMappedVolumeImpl<float> >
OmVolDataSrcs;

typedef boost::variant<int8_t*, uint8_t*, int32_t*, uint32_t*, float*>
OmRawDataPtrs;

class OmVolumeTypeHelpers {
public:
    static std::string GetTypeAsString(const OmVolDataType type);
    static QString GetTypeAsQString(const OmVolDataType type);

    static OmVolDataType GetTypeFromString(const QString & type);

    static int getHDF5FileType(const OmVolDataType type);
    static int getHDF5MemoryType(const OmVolDataType type);
};

