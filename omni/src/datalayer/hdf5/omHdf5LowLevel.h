#ifndef OM_HDF5_LOW_LEVEL_H
#define OM_HDF5_LOW_LEVEL_H

#ifdef WIN32
#include <windows.h>
#include <BaseTsd.h>
typedef LONG_PTR ssize_t;
#endif

#include "common/omCommon.h"
#include "datalayer/omDataWrapper.h"

#include "hdf5.h"

class vtkImageData;

class OmHdf5LowLevel
{
 public:
	//file
	void om_hdf5_file_create(string fpath);
	hid_t om_hdf5_file_open_with_lock(string fpath, const bool readOnly);
	void om_hdf5_file_close_with_lock(hid_t fileId);
	void om_hdf5_flush_with_lock(const hid_t fileId);

	//group
	bool om_hdf5_group_exists_with_lock(hid_t fileId, const char* name);
	void om_hdf5_group_delete_with_lock(hid_t fileId, const char* name);

	//data set
	bool om_hdf5_dataset_exists_with_lock(hid_t fileId, const char* name);
	void om_hdf5_dataset_image_create_tree_overwrite_with_lock(hid_t fileId, const char* name, Vector3<int>* dataDims, Vector3<int>* chunkDims, int bytesPerSample);
	vtkImageData * om_hdf5_dataset_image_read_trim_with_lock(hid_t fileId, const char* name, DataBbox dataExtent, int bytesPerSample);
	void om_hdf5_dataset_image_write_trim_with_lock(hid_t fileId, const char* name, DataBbox* dataExtent, int bytesPerSample, vtkImageData *pImageData);
	void om_hdf5_dataset_delete_create_tree_with_lock(hid_t fileId, const char *name);

	//data set raw
	OmDataWrapperPtr om_hdf5_dataset_raw_read_with_lock(hid_t fileId, const char* name, int* size = NULL);
	void om_hdf5_dataset_raw_create_tree_overwrite_with_lock(hid_t fileId, const char* name, int size, const void* data);
	void om_hdf5_dataset_raw_create_with_lock(hid_t fileId, const char *name, int size, const void *data);

	//image I/O
	Vector3 < int > om_hdf5_dataset_image_get_dims_with_lock(hid_t fileId, const char *name);
	void om_hdf5_dataset_image_create_with_lock(hid_t fileId, const char *name, Vector3<int>* dataDims, Vector3<int>* chunkDims, int bytesPerSample);
	OmDataWrapperPtr om_hdf5_dataset_read_raw_chunk_data(hid_t fileId, const char *name, DataBbox extent, int bytesPerSample);
	void om_hdf5_dataset_write_raw_chunk_data(hid_t fileId, const char *name, DataBbox extent, int bytesPerSample,  void * imageData);
	Vector3< int > om_hdf5_dataset_get_dims_with_lock(hid_t fileId, const char *name);

 private:
	hid_t om_hdf5_bytesToHdf5Id(int bytes);
	void printfDatasetCacheSize( const hid_t dataset_id );
	void printfFileCacheSize( const hid_t fileId );

	//image I/O private
	vtkImageData * om_hdf5_dataset_image_read_with_lock(hid_t fileId, const char *name, DataBbox extent, int bytesPerSample);
	void om_hdf5_dataset_image_write_with_lock(hid_t fileId, const char *name, DataBbox* extent, int bytesPerSample,  vtkImageData * imageData);

	//group private
	void om_hdf5_group_create_with_lock(hid_t fileId, const char *name);
	void om_hdf5_group_create_tree_with_lock(hid_t fileId, const char *name);

	//data set private
	void om_hdf5_dataset_delete_with_lock(hid_t fileId, const char *name);
};
#endif
