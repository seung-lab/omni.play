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
	OmDataWrapperPtr readChunkNotOnBoundary(DataBbox dataExtent);
	void dataset_image_write_trim(const DataBbox&,
				      OmDataWrapperPtr data);
	void dataset_delete_create_tree();

	//data set raw
	OmDataWrapperPtr readDataset(int* size = NULL);
	void allocateDataset(int size, OmDataWrapperPtr data);

	//image I/O
	Vector3i  dataset_image_get_dims();
	void allocateChunkedDataset(const Vector3i&,
				  const Vector3i&,
				  const OmVolDataType);
	OmDataWrapperPtr readChunk(DataBbox extent);
	void writeChunk(DataBbox extent, OmDataWrapperPtr data);
	Vector3i dataset_get_dims();

 private:
	const hid_t fileId;
	OmDataPath path;

	//image I/O private
	OmDataWrapperPtr readChunkVTK(DataBbox extent);

	//data set private
	void dataset_delete();

	OmDataWrapperPtr getNullDataWrapper(hid_t dstype);
	OmDataWrapperPtr getDataWrapper(void*, hid_t, const OmDataAllocType);
	int getSizeofType(hid_t dstype);
	void printTypeInfo( hid_t dstype );
};
#endif
