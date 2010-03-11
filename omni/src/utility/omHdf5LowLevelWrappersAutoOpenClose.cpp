#include "omHdf5LowLevelWrappersAutoOpenClose.h"
#include "common/omException.h"
#include "common/omDebug.h"

#define HDF5_WRAP() 								\
	string pathStr = path.getString();                                      \
	const char * name = pathStr.c_str();                                    \
	hid_t fileId;                                                           \
	bool opened = false;						        \
	try { 									\
		fileId = hdfLowLevel.om_hdf5_file_open_with_lock (mFileName);   \
		opened = true;

#define HDF5_UNWRAP() 								\
	} catch (...) { 							\
	        if(opened) {                                                    \
 			hdfLowLevel.om_hdf5_file_close_with_lock (fileId);	\
		}                                                               \
		throw new OmIoException("died in HDF5!\n");		        \
	}									\
	hdfLowLevel.om_hdf5_file_close_with_lock (fileId);			

OmHdf5LowLevelWrappersAutoOpenClose::OmHdf5LowLevelWrappersAutoOpenClose(string fileName)
	: mFileName(fileName)
{
}

OmHdf5LowLevelWrappersAutoOpenClose::~OmHdf5LowLevelWrappersAutoOpenClose()
{
}

void OmHdf5LowLevelWrappersAutoOpenClose::open()
{
}

void OmHdf5LowLevelWrappersAutoOpenClose::close()
{
}

/////////////////////////////////
///////          File
void OmHdf5LowLevelWrappersAutoOpenClose::file_create()
{
	hdfLowLevel.om_hdf5_file_create(mFileName);
}

/////////////////////////////////
///////          Group
bool OmHdf5LowLevelWrappersAutoOpenClose::group_exists_with_lock(OmHdf5Path path)
{
	bool exists;
	HDF5_WRAP();
	exists = hdfLowLevel.om_hdf5_group_exists_with_lock (fileId, name);
	HDF5_UNWRAP();
	return exists;
}

void OmHdf5LowLevelWrappersAutoOpenClose::group_delete_with_lock(OmHdf5Path path)
{
	HDF5_WRAP();
	hdfLowLevel.om_hdf5_group_delete_with_lock(fileId, name);
	HDF5_UNWRAP();
}

/////////////////////////////////
///////          Dataset

bool OmHdf5LowLevelWrappersAutoOpenClose::dataset_exists_with_lock(OmHdf5Path path)
{
	bool exists;
	HDF5_WRAP();
	exists = hdfLowLevel.om_hdf5_dataset_exists_with_lock (fileId, name);
	HDF5_UNWRAP();
	return exists;
}

void OmHdf5LowLevelWrappersAutoOpenClose::dataset_image_create_tree_overwrite_with_lock(OmHdf5Path path, Vector3<int>* dataDims, Vector3< int>* chunkDims, int bytesPerSample, bool unlimited)
{
	HDF5_WRAP();
	hdfLowLevel.om_hdf5_dataset_delete_create_tree_with_lock(fileId, name);
	hdfLowLevel.om_hdf5_dataset_image_create_with_lock(fileId, name, dataDims, chunkDims, bytesPerSample, unlimited);
	HDF5_UNWRAP();
}

vtkImageData * OmHdf5LowLevelWrappersAutoOpenClose::dataset_image_read_trim_with_lock(OmHdf5Path path, DataBbox dataExtent, int bytesPerSample)
{
	vtkImageData * ret;
	HDF5_WRAP();
	ret = hdfLowLevel.om_hdf5_dataset_image_read_trim_with_lock(fileId, name, dataExtent, bytesPerSample);
	HDF5_UNWRAP();
	return ret;
}

void OmHdf5LowLevelWrappersAutoOpenClose::dataset_image_write_trim_with_lock(OmHdf5Path path, DataBbox* dataExtent, int bytesPerSample, vtkImageData * pImageData)
{
	HDF5_WRAP();
	hdfLowLevel.om_hdf5_dataset_image_write_trim_with_lock(fileId, name, dataExtent, bytesPerSample, pImageData);
	HDF5_UNWRAP();
}

void * OmHdf5LowLevelWrappersAutoOpenClose::dataset_raw_read_with_lock(OmHdf5Path path, int *size)
{
	void * ret;
	HDF5_WRAP();
	ret = hdfLowLevel.om_hdf5_dataset_raw_read_with_lock(fileId, name, size);
	HDF5_UNWRAP();
	return ret;
}

void OmHdf5LowLevelWrappersAutoOpenClose::dataset_raw_create_with_lock(OmHdf5Path path, int size, const void *data)
{
	HDF5_WRAP();
	hdfLowLevel.om_hdf5_dataset_raw_create_with_lock(fileId, name, size, data);
	HDF5_UNWRAP();
}

void OmHdf5LowLevelWrappersAutoOpenClose::dataset_raw_create_tree_overwrite_with_lock(OmHdf5Path path, int size, const void* data)
{
	HDF5_WRAP();
	hdfLowLevel.om_hdf5_dataset_raw_create_tree_overwrite_with_lock(fileId, name, size, data);
	HDF5_UNWRAP();
}

//imageIo
Vector3 < int > OmHdf5LowLevelWrappersAutoOpenClose::dataset_image_get_dims_with_lock(OmHdf5Path path)
{
	Vector3<int> ret;
	HDF5_WRAP();
	ret = hdfLowLevel.om_hdf5_dataset_image_get_dims_with_lock(fileId, name);
	HDF5_UNWRAP();
	return ret;
}
