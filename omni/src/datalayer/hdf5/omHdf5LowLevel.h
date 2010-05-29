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
	static void om_hdf5_file_create(string fpath);
	static hid_t om_hdf5_file_open_with_lock(string fpath, const bool readOnly);
	static void om_hdf5_file_close_with_lock(hid_t fileId);
	static void om_hdf5_flush_with_lock(const hid_t fileId);

	//group
	static bool om_hdf5_group_exists_with_lock(hid_t fileId, const char* name);
	static void om_hdf5_group_delete_with_lock(hid_t fileId, const char* name);

	//data set
	static bool om_hdf5_dataset_exists_with_lock(hid_t fileId, const char* name);
	static void om_hdf5_dataset_image_create_tree_overwrite_with_lock(hid_t fileId, const char* name, Vector3<int>* dataDims, Vector3<int>* chunkDims, int bytesPerSample);
	static vtkImageData * om_hdf5_dataset_image_read_trim_with_lock(hid_t fileId, const char* name, DataBbox dataExtent, int bytesPerSample);
	static void om_hdf5_dataset_image_write_trim_with_lock(hid_t fileId, const char* name, DataBbox* dataExtent, int bytesPerSample, vtkImageData *pImageData);
	static void om_hdf5_dataset_delete_create_tree_with_lock(hid_t fileId, const char *name);

	//data set raw
	static OmDataWrapperPtr om_hdf5_dataset_raw_read_with_lock(hid_t fileId, const char* name, int* size = NULL);
	static void om_hdf5_dataset_raw_create_tree_overwrite_with_lock(hid_t fileId, const char* name, int size, const void* data);
	static void om_hdf5_dataset_raw_create_with_lock(hid_t fileId, const char *name, int size, const void *data);

	//image I/O
	static Vector3 < int > om_hdf5_dataset_image_get_dims_with_lock(hid_t fileId, const char *name);
	static void om_hdf5_dataset_image_create_with_lock(hid_t fileId, const char *name, Vector3<int>* dataDims, Vector3<int>* chunkDims, int bytesPerSample);
	static OmDataWrapperPtr om_hdf5_dataset_read_raw_chunk_data(hid_t fileId, const char *name, DataBbox extent, int bytesPerSample);
	static void om_hdf5_dataset_write_raw_chunk_data(hid_t fileId, const char *name, DataBbox extent, int bytesPerSample,  void * imageData);
	static Vector3< int > om_hdf5_dataset_get_dims_with_lock(hid_t fileId, const char *name);

 private:
	static hid_t om_hdf5_bytesToHdf5Id(int bytes);
	static void printfDatasetCacheSize( const hid_t dataset_id );
	static void printfFileCacheSize( const hid_t fileId );
	static void printTypeInfo( hid_t dstype );

	//image I/O private
	static vtkImageData * om_hdf5_dataset_image_read_with_lock(hid_t fileId, const char *name, DataBbox extent, int bytesPerSample);
	static void om_hdf5_dataset_image_write_with_lock(hid_t fileId, const char *name, DataBbox* extent, int bytesPerSample,  vtkImageData * imageData);

	//group private
	static void om_hdf5_group_create_with_lock(hid_t fileId, const char *name);
	static void om_hdf5_group_create_tree_with_lock(hid_t fileId, const char *name);

	//data set private
	static void om_hdf5_dataset_delete_with_lock(hid_t fileId, const char *name);
};
#endif
