#ifndef OM_MDF5_LOW_LEVEL_H
#define OM_MDF5_LOW_LEVEL_H

#include "volume/omVolumeTypes.h"
#include "common/omStd.h"
#include "common/omSerialization.h"

#include <vmmlib/vmmlib.h>
using namespace vmml;

#include <hdf5.h>

class vtkImageData;

class OmHdf5LowLevel
{
 public:
	//file
	void om_hdf5_file_create(string fpath);
	hid_t om_hdf5_file_open_with_lock(string fpath);
	void om_hdf5_file_close_with_lock(hid_t fileId);

	//group
	bool om_hdf5_group_exists_with_lock(hid_t fileId, const char* name);
	void om_hdf5_group_delete_with_lock(hid_t fileId, const char* name);

	//data set
	bool om_hdf5_dataset_exists_with_lock(hid_t fileId, const char* name);
	void om_hdf5_dataset_image_create_tree_overwrite_with_lock(hid_t fileId, const char* name, Vector3<int> dataDims, Vector3<int> chunkDims, int bytesPerSample, bool unlimited = false);
	vtkImageData * om_hdf5_dataset_image_read_trim_with_lock(hid_t fileId, const char* name, DataBbox dataExtent, int bytesPerSample);
	void om_hdf5_dataset_image_write_trim_with_lock(hid_t fileId, const char* name, DataBbox dataExtent, int bytesPerSample, vtkImageData *pImageData);
	void om_hdf5_dataset_delete_create_tree_with_lock(hid_t fileId, const char *name);

	//data set raw
	void * om_hdf5_dataset_raw_read_with_lock(hid_t fileId, const char* name, int* size = NULL);
	void om_hdf5_dataset_raw_create_tree_overwrite_with_lock(hid_t fileId, const char* name, int size, const void* data);
	void om_hdf5_dataset_raw_create_with_lock(hid_t fileId, const char *name, int size, const void *data);

	//imageIo
	Vector3 < int > om_hdf5_dataset_image_get_dims_with_lock(hid_t fileId, const char *name);
	void om_hdf5_dataset_image_create_with_lock(hid_t fileId, const char *name, Vector3 < int >dataDims, Vector3 < int >chunkDims, int bytesPerSample, bool unlimited);

 private:
	hid_t om_hdf5_bytesToHdf5Id(int bytes);

	//imageIo
	vtkImageData * om_hdf5_dataset_image_read_with_lock(hid_t fileId, const char *name, DataBbox extent, int bytesPerSample);
	void om_hdf5_dataset_image_write_with_lock(hid_t fileId, const char *name, DataBbox extent, int bytesPerSample,  vtkImageData * imageData);

	//group private
	void om_hdf5_group_create_with_lock(hid_t fileId, const char *name);
	void om_hdf5_group_create_tree_with_lock(hid_t fileId, const char *name);

	//data set
	void om_hdf5_dataset_delete_with_lock(hid_t fileId, const char *name);
};
#endif
