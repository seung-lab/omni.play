#pragma once
#include "precomp.h"

#include "common/common.h"
#include "common/enums.hpp"
#include "boost/variant.hpp"
#include "enum/enum.hpp"

template <typename T>
class OmMemMappedVolumeImpl;

class OmChannel;
class OmSegmentation;

typedef boost::variant<
    OmMemMappedVolumeImpl<int8_t>, OmMemMappedVolumeImpl<uint8_t>,
    OmMemMappedVolumeImpl<int32_t>, OmMemMappedVolumeImpl<uint32_t>,
    OmMemMappedVolumeImpl<float> > OmVolDataSrcs;

typedef boost::variant<int8_t*, uint8_t*, int32_t*, uint32_t*, float*>
    OmRawDataPtrs;

class OmVolumeTypeHelpers {
 public:
  static std::string GetTypeAsString(const om::common::DataType type);
  static QString GetTypeAsQString(const om::common::DataType type);

  static om::common::DataType GetTypeFromString(const QString& type);

  static int getHDF5FileType(const om::common::DataType type);
  static int getHDF5MemoryType(const om::common::DataType type);
};
