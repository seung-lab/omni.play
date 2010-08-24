#ifndef OM_HDF5_LOW_LEVEL_H
#define OM_HDF5_LOW_LEVEL_H

#ifdef WIN32
#include <windows.h>
#include <BaseTsd.h>
typedef LONG_PTR ssize_t;
#endif

#include "common/omCommon.h"
#include "datalayer/omDataWrapper.h"
#include "volume/omVolumeTypes.hpp"
#include "datalayer/omDataPath.h"

#include "hdf5.h"

class OmHdf5LowLevel {
public:
	explicit OmHdf5LowLevel(hid_t id)
		: fileId(id) {}

	void setPath(const OmDataPath & p){
		path = p;
	}

	const char* getPath(){
		return path.getString().c_str();
	}

	//group
	bool group_exists(const char*);
	void group_delete(const char*);
	void group_create(const char*);
	bool group_exists();
	void group_delete();
	void group_create();
	void group_create_tree(const char*);

	//data set
	bool dataset_exists();
	OmDataWrapperPtr dataset_image_read_trim(DataBbox dataExtent);
	void dataset_image_write_trim(const DataBbox&,
				      OmDataWrapperPtr data);
	void dataset_delete_create_tree();

	//data set raw
	OmDataWrapperPtr dataset_raw_read(int* size = NULL);
	void dataset_raw_create(int size, OmDataWrapperPtr data);

	//image I/O
	Vector3i  dataset_image_get_dims();
	void dataset_image_create(const Vector3i&,
				  const Vector3i&,
				  const OmVolDataType);
	OmDataWrapperPtr dataset_read_raw_chunk_data(DataBbox extent);
	void dataset_write_raw_chunk_data(DataBbox extent, OmDataWrapperPtr data);
	Vector3i dataset_get_dims();

 private:
	const hid_t fileId;
	OmDataPath path;

	//image I/O private
	OmDataWrapperPtr dataset_image_read(DataBbox extent);

	//data set private
	void dataset_delete();

	OmDataWrapperPtr getNullDataWrapper(hid_t dstype);
	OmDataWrapperPtr getDataWrapper(void*, hid_t, const OmDataAllocType);
	int getSizeofType(hid_t dstype);
	void printTypeInfo( hid_t dstype );
};
#endif
