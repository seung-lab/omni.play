#include "datalayer/hdf5/omHdf5Utils.hpp"
#include "common/omException.h"
#include "datalayer/omDataPath.h"

#include "hdf5.h"

void OmHdf5Utils::group_create(const int fileId,
							   const char* path)
{
	//Creates a new empty group and links it into the file.
	hid_t group_id = H5Gcreate2(fileId, path, 0, H5P_DEFAULT, H5P_DEFAULT);
	if (group_id < 0) {
		throw OmIoException("Could not create HDF5 group \""
							+ std::string(path) + "\"");
	}

	herr_t status = H5Gclose(group_id);
	if (status < 0) {
		throw OmIoException("Could not close HDF5 group.");
	}
}

void OmHdf5Utils::group_delete(const int fileId, const char* path)
{
	herr_t err = H5Gunlink(fileId, path);
	if (err < 0) {
		throw OmIoException("Could not unlink HDF5 group " + std::string(path));
	}
}

bool OmHdf5Utils::group_exists(const int fileId, const char* path)
{
	H5E_BEGIN_TRY {
 		herr_t ret = H5Gget_objinfo(fileId, path, 0, NULL);
 		if( ret < 0 ){
 			return false;
 		}
 	} H5E_END_TRY

		  return true;
}

bool OmHdf5Utils::dataset_exists(const int fileId, const char* path)
{
	hid_t dataset_id;

	//Try to open a data set
	//Turn off error printing idea from http://www.fiberbundle.net/index.html
	H5E_BEGIN_TRY {
		dataset_id = H5Dopen2(fileId, path, H5P_DEFAULT);
	} H5E_END_TRY

		  //if failure, then assume doesn't exist
		  if (dataset_id < 0)
			  return false;

	//Closes the specified dataset.
	herr_t ret = H5Dclose(dataset_id);
	if (ret < 0) {
		throw OmIoException("Could not close HDF5 dataset "
							+ std::string(path));
	}

	return true;
}

void OmHdf5Utils::dataset_delete(const int fileId, const char* path)
{
	//Removes the link to an object from a group.
	herr_t err = H5Gunlink(fileId, path);
	if (err < 0) {
		throw OmIoException("Could not unlink HDF5 dataset "
							+ std::string(path));
	}
}

void OmHdf5Utils::dataset_delete_if_exists(const int fileId, const char* path)
{
	if (dataset_exists(fileId, path)) {
		dataset_delete(fileId, path);
	}
}

OmDataWrapperPtr OmHdf5Utils::getNullDataWrapper(const int dstype)
{
	OmDataWrapperPtr dw = getDataWrapper(NULL, dstype, om::NONE);
	return dw;
}

void OmHdf5Utils::printTypeInfo(const int dstype)
{
	OmDataWrapperPtr dw = getNullDataWrapper(dstype);
	std::string type = dw->getTypeAsString();
	printf("type is %s\n", type.c_str() );
}

int OmHdf5Utils::getSizeofType(const int dstype)
{
	OmDataWrapperPtr dw = getNullDataWrapper(dstype);
	const int size = dw->getSizeof();
	return size;
}

OmDataWrapperPtr OmHdf5Utils::getDataWrapper(void* dataset,
											 const int dstype,
											 const om::OmDataAllocType allocType)
{
	switch( H5Tget_class( dstype ) ){
	case H5T_INTEGER:
		if( H5Tequal(dstype, H5T_NATIVE_UCHAR ) ){
			return OmDataWrapper<uint8_t>::produce(dataset, allocType);
		} else if( H5Tequal(dstype, H5T_NATIVE_CHAR ) ){
			return OmDataWrapper<int8_t>::produce(dataset, allocType);
		}else if( H5Tequal(dstype, H5T_NATIVE_UINT ) ){
			return OmDataWrapper<uint32_t>::produce(dataset, allocType);
		}else if( H5Tequal(dstype, H5T_NATIVE_INT ) ){
			return OmDataWrapper<int32_t>::produce(dataset, allocType);
		}else {
			throw OmIoException("unknown hdf5 integer type");
		}
	case H5T_FLOAT:
		return OmDataWrapper<float>::produce(dataset, allocType);
	default:
		throw OmIoException("unknown hdf5 type");
	}
}

