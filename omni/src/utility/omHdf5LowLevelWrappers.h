#ifndef OM_MDF5_LOW_LEVEL_WRAPPERS_H
#define OM_MDF5_LOW_LEVEL_WRAPPERS_H

#include "utility/omHdf5LowLevel.h"

class OmHdf5LowLevelWrappers
{
 public:
	//file
	void file_create(string fpath);
	
	//group
	bool group_exists_with_lock(string fileName, const char* name);
	void group_delete_with_lock(string fileName, const char* name);
	
	//data set
	bool dataset_exists_with_lock(string fileName, const char* name);

	//image I/O
	Vector3 < int > dataset_image_get_dims_with_lock(string fileName, const char *name);
	void dataset_image_create_tree_overwrite_with_lock(string fileName, const char* name, Vector3<int> dataDims, Vector3<int> chunkDims, int bytesPerSample, bool unlimited);
	vtkImageData* dataset_image_read_trim_with_lock(string fileName, const char* name, DataBbox dataExtent, int bytesPerSample);
	void dataset_image_write_trim_with_lock(string fileName, const char* name, DataBbox dataExtent, int bytesPerSample, vtkImageData *pImageData);
	
	//data set raw
	void* dataset_raw_read_with_lock(string fileName, const char* name, int* size = NULL);
	void dataset_raw_create_with_lock(string fileName, const char *name, int size, const void *data);
	void dataset_raw_create_tree_overwrite_with_lock(string fileName, const char* name, int size, const void* data);

 private:
	OmHdf5LowLevel hdfLowLevel;
};

#endif
