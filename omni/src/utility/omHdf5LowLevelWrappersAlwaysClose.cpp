#include "omHdf5LowLevelWrappersAlwaysClose.h"
#include "common/omException.h"
#include "common/omDebug.h"

#define HDF5_WRAP() 								\
	hid_t fileId;                                                           \
	bool opened = false;						        \
	try { 									\
		fileId = hdfLowLevel.om_hdf5_file_open_with_lock (fileName);    \
		opened = true;

#define HDF5_UNWRAP() 								\
	} catch (...) { 							\
	        if(opened) {                                                    \
 			hdfLowLevel.om_hdf5_file_close_with_lock (fileId);	\
		}                                                               \
		throw new OmIoException("died in HDF5!\n");		        \
	}									\
	hdfLowLevel.om_hdf5_file_close_with_lock (fileId);			

OmHdf5LowLevelWrappersAlwaysClose::OmHdf5LowLevelWrappersAlwaysClose()
{
}

void OmHdf5LowLevelWrappersAlwaysClose::open(string fileName)
{
}

void OmHdf5LowLevelWrappersAlwaysClose::close(string fileName)
{
}

/////////////////////////////////
///////          File
void OmHdf5LowLevelWrappersAlwaysClose::file_create(string fpath)
{
	hdfLowLevel.om_hdf5_file_create( fpath );
}

/////////////////////////////////
///////          Group
bool OmHdf5LowLevelWrappersAlwaysClose::group_exists_with_lock(string fileName, const char *name)
{
	bool exists;
	HDF5_WRAP();
	exists = hdfLowLevel.om_hdf5_group_exists_with_lock (fileId, name);
	HDF5_UNWRAP();
	return exists;
}

void OmHdf5LowLevelWrappersAlwaysClose::group_delete_with_lock(string fileName, const char *name)
{
	HDF5_WRAP();
	hdfLowLevel.om_hdf5_group_delete_with_lock(fileId, name);
	HDF5_UNWRAP();
}

/////////////////////////////////
///////          Dataset

bool OmHdf5LowLevelWrappersAlwaysClose::dataset_exists_with_lock(string fileName, const char *name)
{
	bool exists;
	HDF5_WRAP();
	exists = hdfLowLevel.om_hdf5_dataset_exists_with_lock (fileId, name);
	HDF5_UNWRAP();
	return exists;
}

void OmHdf5LowLevelWrappersAlwaysClose::dataset_image_create_tree_overwrite_with_lock(string fileName, const char *name, Vector3 < int >dataDims,
					    Vector3 < int >chunkDims, int bytesPerSample, bool unlimited)
{
	HDF5_WRAP();
	hdfLowLevel.om_hdf5_dataset_delete_create_tree_with_lock(fileId, name);
	hdfLowLevel.om_hdf5_dataset_image_create_with_lock(fileId, name, dataDims, chunkDims, bytesPerSample, unlimited);
	HDF5_UNWRAP();
}

vtkImageData * OmHdf5LowLevelWrappersAlwaysClose::dataset_image_read_trim_with_lock(string fileName, const char *name, DataBbox dataExtent, int bytesPerSample)
{
	vtkImageData * ret;
	HDF5_WRAP();
	ret = hdfLowLevel.om_hdf5_dataset_image_read_trim_with_lock(fileId, name, dataExtent, bytesPerSample);
	HDF5_UNWRAP();
	return ret;
}

void OmHdf5LowLevelWrappersAlwaysClose::dataset_image_write_trim_with_lock(string fileName, const char *name, DataBbox dataExtent, int bytesPerSample,
				 vtkImageData * pImageData)
{
	HDF5_WRAP();
	hdfLowLevel.om_hdf5_dataset_image_write_trim_with_lock(fileId, name, dataExtent, bytesPerSample, pImageData);
	HDF5_UNWRAP();
}

void * OmHdf5LowLevelWrappersAlwaysClose::dataset_raw_read_with_lock(string fileName, const char *name, int *size)
{
	void * ret;
	HDF5_WRAP();
	ret = hdfLowLevel.om_hdf5_dataset_raw_read_with_lock(fileId, name, size);
	HDF5_UNWRAP();
	return ret;
}

void OmHdf5LowLevelWrappersAlwaysClose::dataset_raw_create_with_lock(string fileName, const char *name, int size, const void *data)
{
	HDF5_WRAP();
	hdfLowLevel.om_hdf5_dataset_raw_create_with_lock(fileId, name, size, data);
	HDF5_UNWRAP();
}

void OmHdf5LowLevelWrappersAlwaysClose::dataset_raw_create_tree_overwrite_with_lock(string fileName, const char* name, int size, const void* data)
{
	HDF5_WRAP();
	hdfLowLevel.om_hdf5_dataset_raw_create_tree_overwrite_with_lock(fileId, name, size, data);
	HDF5_UNWRAP();
}

//imageIo
Vector3 < int > OmHdf5LowLevelWrappersAlwaysClose::dataset_image_get_dims_with_lock(string fileName, const char *name)
{
	HDF5_WRAP();
	hdfLowLevel.om_hdf5_dataset_image_get_dims_with_lock(fileId, name);
	HDF5_UNWRAP();
}
