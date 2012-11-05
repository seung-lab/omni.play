#include "volume/omVolumeTypes.hpp"
#include "hdf5.h"

// hid_t is typedef'd to int in H5Ipublic.h
int OmVolumeTypeHelpers::getHDF5FileType(const om::common::DataType type)
{
    switch(type.index()){
    case om::common::DataType::INT8:
        return H5T_STD_I8LE;
    case om::common::DataType::UINT8:
        return H5T_STD_U8LE;
    case om::common::DataType::INT32:
        return H5T_STD_I32LE;
    case om::common::DataType::UINT32:
        return H5T_STD_U32LE;
    case om::common::DataType::FLOAT:
        return H5T_IEEE_F32LE;
    case om::common::DataType::UNKNOWN:
    default:
        throw IoException("unknown data type");
    }
}

// hid_t is typedef'd to int in H5Ipublic.h
int OmVolumeTypeHelpers::getHDF5MemoryType(const om::common::DataType type)
{
    switch(type.index()){
    case om::common::DataType::INT8:
        return H5T_NATIVE_CHAR;
    case om::common::DataType::UINT8:
        return H5T_NATIVE_UCHAR;
    case om::common::DataType::INT32:
        return H5T_NATIVE_INT;
    case om::common::DataType::UINT32:
        return H5T_NATIVE_UINT;
    case om::common::DataType::FLOAT:
        return H5T_NATIVE_FLOAT;
    case om::common::DataType::UNKNOWN:
    default:
        throw IoException("unknown data type");
    }
}

std::string OmVolumeTypeHelpers::GetTypeAsString(const om::common::DataType type){
    return type.value();
}

QString OmVolumeTypeHelpers::GetTypeAsQString(const om::common::DataType type){
    return QString::fromStdString(GetTypeAsString(type));
}

om::common::DataType OmVolumeTypeHelpers::GetTypeFromString(const QString & type)
{
    boost::optional<om::common::DataType> ret =
        om::common::DataType::get_by_value(type.toStdString());

    if(ret){
        return *ret;
    }

    throw IoException("invalid type", type);
}

