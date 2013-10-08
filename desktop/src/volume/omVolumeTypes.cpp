#include "volume/omVolumeTypes.hpp"
#include "hdf5.h"

// hid_t is typedef'd to int in H5Ipublic.h
int OmVolumeTypeHelpers::getHDF5FileType(const OmVolDataType type) {
  switch (type.index()) {
    case OmVolDataType::INT8:
      return H5T_STD_I8LE;
    case OmVolDataType::UINT8:
      return H5T_STD_U8LE;
    case OmVolDataType::INT32:
      return H5T_STD_I32LE;
    case OmVolDataType::UINT32:
      return H5T_STD_U32LE;
    case OmVolDataType::FLOAT:
      return H5T_IEEE_F32LE;
    case OmVolDataType::UNKNOWN:
    default:
      throw om::IoException("unknown data type");
  }
}

// hid_t is typedef'd to int in H5Ipublic.h
int OmVolumeTypeHelpers::getHDF5MemoryType(const OmVolDataType type) {
  switch (type.index()) {
    case OmVolDataType::INT8:
      return H5T_NATIVE_CHAR;
    case OmVolDataType::UINT8:
      return H5T_NATIVE_UCHAR;
    case OmVolDataType::INT32:
      return H5T_NATIVE_INT;
    case OmVolDataType::UINT32:
      return H5T_NATIVE_UINT;
    case OmVolDataType::FLOAT:
      return H5T_NATIVE_FLOAT;
    case OmVolDataType::UNKNOWN:
    default:
      throw om::IoException("unknown data type");
  }
}

std::string OmVolumeTypeHelpers::GetTypeAsString(const OmVolDataType type) {
  return type.value();
}

QString OmVolumeTypeHelpers::GetTypeAsQString(const OmVolDataType type) {
  return QString::fromStdString(GetTypeAsString(type));
}

OmVolDataType OmVolumeTypeHelpers::GetTypeFromString(const QString& type) {
  boost::optional<OmVolDataType> ret =
      OmVolDataType::get_by_value(type.toStdString());

  if (ret) {
    return *ret;
  }

  throw om::IoException("invalid type");
}
