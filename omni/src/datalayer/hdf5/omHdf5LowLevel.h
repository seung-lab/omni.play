#ifndef LOW_LEVEL_H
#define LOW_LEVEL_H

#ifdef WIN32
#include <windows.h>
#include <BaseTsd.h>
typedef LONG_PTR ssize_t;
#endif

#include "common/omCommon.h"
#include "datalayer/omDataWrapper.h"
#include "volume/omVolumeTypes.hpp"

#include "hdf5.h"

class vtkImageData;

class OmHdf5LowLevel
{
 public:

	//file
	static void file_create(string fpath);
	static hid_t file_open(string fpath, const bool readOnly);
	static void file_close(hid_t fileId);
	static void flush(const hid_t fileId);

	//group
	static bool group_exists(hid_t fileId, const char* name);
	static void group_delete(hid_t fileId, const char* name);

	//data set
	static bool dataset_exists(hid_t fileId, const char* name);
	static OmDataWrapperPtr dataset_image_read_trim(hid_t fileId, const char* name, DataBbox dataExtent);
	static void dataset_image_write_trim(hid_t, const char*,
							       const DataBbox&,
							       OmDataWrapperPtr data);
	static void dataset_delete_create_tree(hid_t fileId, const char *name);

	//data set raw
	static OmDataWrapperPtr dataset_raw_read(hid_t fileId, const char* name, int* size = NULL);
	static void dataset_raw_create(hid_t fileId, const char *name, int size, OmDataWrapperPtr data);

	//image I/O
	static Vector3i  dataset_image_get_dims(hid_t fileId, const char *name);
	static void dataset_image_create(hid_t , const char *,
							   const Vector3i&,
							   const Vector3i&,
							   const OmVolDataType);
	static OmDataWrapperPtr dataset_read_raw_chunk_data(hid_t fileId, const char *name, DataBbox extent);
	static void dataset_write_raw_chunk_data(hid_t fileId, const char *name, DataBbox extent, OmDataWrapperPtr data);
	static Vector3i dataset_get_dims(hid_t fileId, const char *name);

 private:
	static void printfDatasetCacheSize( const hid_t dataset_id );
	static void printfFileCacheSize( const hid_t fileId );

	//image I/O private
	static OmDataWrapperPtr dataset_image_read(hid_t fileId, const char *name, DataBbox extent);

	//group private
	static void group_create(hid_t fileId, const char *name);
	static void group_create_tree(hid_t fileId, const char *name);

	//data set private
	static void dataset_delete(hid_t fileId, const char *name);

	static OmDataWrapperPtr getNullDataWrapper(hid_t dstype );
	static OmDataWrapperPtr getDataWrapper(void*, hid_t, const OmDataAllocType);
	static int getSizeofType(hid_t dstype);
	static void printTypeInfo( hid_t dstype );
};
#endif
