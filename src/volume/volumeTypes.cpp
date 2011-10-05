#include "volume/volumeTypes.h"

using namespace om::common;

namespace om {
namespace volume {

// hid_t is typedef'd to int in H5Ipublic.h
int volumeTypeHelpers::getHDF5FileType(const volDataType type)
{
    switch(type.index()){
    case volDataType::INT8:
        return H5T_STD_I8LE;
    case volDataType::UINT8:
        return H5T_STD_U8LE;
    case volDataType::INT32:
        return H5T_STD_I32LE;
    case volDataType::UINT32:
        return H5T_STD_U32LE;
    case volDataType::FLOAT:
        return H5T_IEEE_F32LE;
    case volDataType::UNKNOWN:
    default:
        throw ioException("unknown data type");
    }
}

// hid_t is typedef'd to int in H5Ipublic.h
int volumeTypeHelpers::getHDF5MemoryType(const volDataType type)
{
    switch(type.index()){
    case volDataType::INT8:
        return H5T_NATIVE_CHAR;
    case volDataType::UINT8:
        return H5T_NATIVE_UCHAR;
    case volDataType::INT32:
        return H5T_NATIVE_INT;
    case volDataType::UINT32:
        return H5T_NATIVE_UINT;
    case volDataType::FLOAT:
        return H5T_NATIVE_FLOAT;
    case volDataType::UNKNOWN:
    default:
        throw ioException("unknown data type");
    }
}

std::string volumeTypeHelpers::GetTypeAsString(const volDataType type){
    return type.value();
}

volDataType volumeTypeHelpers::GetTypeFromString(const std::string & type)
{
    boost::optional<volDataType> ret =
        volDataType::get_by_value(type.toStdString());

    if(ret){
        return *ret;
    }

    throw ioException("invalid type", type);
}

} // namespace volume
} // namespace om