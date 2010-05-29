#ifndef OM_MDF5_LOW_LEVEL_WRAPPERS_MANUAL_OPEN_CLOSE_H
#define OM_MDF5_LOW_LEVEL_WRAPPERS_MANUAL_OPEN_CLOSE_H

#include "datalayer/hdf5/omHdf5LowLevel.h"
#include "datalayer/omDataWrapper.h"

class OmDataPath;

class OmHdf5LowLevelWrappersManualOpenClose
{
 public:
	OmHdf5LowLevelWrappersManualOpenClose(string fileName, const bool readOnly);
	~OmHdf5LowLevelWrappersManualOpenClose();

	void open();
	void close();
	void flush();

	//file
	void file_create();
	
	//group
	bool group_exists_with_lock(const OmDataPath & path);
	void group_delete_with_lock(const OmDataPath & path);
	
	//data set
	bool dataset_exists_with_lock(const OmDataPath & path);

	//image I/O
	Vector3 < int > dataset_image_get_dims_with_lock(const OmDataPath & path);
	void dataset_image_create_tree_overwrite_with_lock(const OmDataPath & path, Vector3<int>* dataDims, Vector3<int>* chunkDims, int bytesPerSample);
	vtkImageData* dataset_image_read_trim_with_lock(const OmDataPath & path, DataBbox dataExtent, int bytesPerSample);
	void dataset_image_write_trim_with_lock(const OmDataPath & path, DataBbox* dataExtent, int bytesPerSample, vtkImageData *pImageData);
	
	//data set raw
	OmDataWrapperPtr dataset_raw_read_with_lock(const OmDataPath & path, int* size = NULL);
	void dataset_raw_create_with_lock(const OmDataPath & path, int size, const void *data);
	void dataset_raw_create_tree_overwrite_with_lock(const OmDataPath & path, int size, const void* data);
	OmDataWrapperPtr dataset_read_raw_chunk_data(const OmDataPath & path, DataBbox dataExtent, int bytesPerSample);
	void dataset_write_raw_chunk_data(const OmDataPath & path, DataBbox dataExtent, int bytesPerSample, void * imageData);
	Vector3< int > dataset_get_dims_with_lock( const OmDataPath & path );

 private:
	OmHdf5LowLevel hdfLowLevel;
	const string mFileName;
	const bool mReadOnly;
	hid_t fileId;
	bool opened;
};

#endif
