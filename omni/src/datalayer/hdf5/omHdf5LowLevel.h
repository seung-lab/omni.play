#ifndef OM_HDF5_LOW_LEVEL_H
#define OM_HDF5_LOW_LEVEL_H

#ifdef WIN32
#include <windows.h>
#include <BaseTsd.h>
typedef LONG_PTR ssize_t;
#endif

#include "common/omCommon.h"
#include "datalayer/omDataWrapper.h"
#include "datalayer/omDataPath.h"

class OmHdf5LowLevel {
public:
	explicit OmHdf5LowLevel(const int id)
		: fileId(id) {}

	void setPath(const OmDataPath & p){
		path = p;
	}

	const char* getPath(){
		return path.getString().c_str();
	}

	//group
	bool group_exists();
	void group_delete();
	void group_create();
	void group_create_tree(const char*);

	bool dataset_exists();
	void dataset_delete_create_tree();
	Vector3i getDatasetDims();
	OmDataWrapperPtr readDataset(int* size = NULL);
	void allocateDataset(int size, OmDataWrapperPtr data);



	Vector3i getChunkedDatasetDims();
	void allocateChunkedDataset(const Vector3i&,
				    const Vector3i&,
				    const OmVolDataType);
	OmDataWrapperPtr readChunkNotOnBoundary(const DataBbox&);
	OmDataWrapperPtr readChunk(const DataBbox&);
	void writeChunk(const DataBbox&, OmDataWrapperPtr);

 private:
	// hid_t is typedef'd to int in H5Ipublic.h
	const int fileId;
	OmDataPath path;

	OmDataWrapperPtr readChunkVTK(DataBbox extent);
	OmDataWrapperPtr readChunkNotOnBoundaryVTK(const DataBbox&,
						   const DataBbox&);

};
#endif
