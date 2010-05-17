#ifndef OM_MDF5_LOW_LEVEL_WRAPPERS_AUTO_OPEN_CLOSE_H
#define OM_MDF5_LOW_LEVEL_WRAPPERS_AUTO_OPEN_CLOSE_H

#include "utility/omHdf5LowLevelWrappersAbstract.h"

class OmHdf5LowLevelWrappersAutoOpenClose : public OmHdf5LowLevelWrappersAbstract
{
 public:
	OmHdf5LowLevelWrappersAutoOpenClose(string fileName, const bool readOnly );
	~OmHdf5LowLevelWrappersAutoOpenClose();

	virtual void open();
	virtual void close();

	//file
	virtual void file_create();
	
	//group
	virtual bool group_exists_with_lock(OmHdf5Path path);
	virtual void group_delete_with_lock(OmHdf5Path path);
	
	//data set
	virtual bool dataset_exists_with_lock(OmHdf5Path path);

	//image I/O
	virtual Vector3 < int > dataset_image_get_dims_with_lock(OmHdf5Path path);
	virtual void dataset_image_create_tree_overwrite_with_lock(OmHdf5Path path, Vector3<int>* dataDims, Vector3<int>* chunkDims, int bytesPerSample);
	virtual vtkImageData* dataset_image_read_trim_with_lock(OmHdf5Path path, DataBbox dataExtent, int bytesPerSample);
	virtual void dataset_image_write_trim_with_lock(OmHdf5Path path, DataBbox* dataExtent, int bytesPerSample, vtkImageData *pImageData);
	virtual void* dataset_read_raw_chunk_data(OmHdf5Path path, DataBbox dataExtent, int bytesPerSample);

	
	//data set raw
	virtual void* dataset_raw_read_with_lock(OmHdf5Path path, int* size = NULL);
	virtual void dataset_raw_create_with_lock(OmHdf5Path path, int size, const void *data);
	virtual void dataset_raw_create_tree_overwrite_with_lock(OmHdf5Path path, int size, const void* data);
	virtual void dataset_write_raw_chunk_data(OmHdf5Path path, DataBbox dataExtent, int bytesPerSample, void * imageData);
	virtual Vector3< int > dataset_get_dims_with_lock( OmHdf5Path path );

 private:
	OmHdf5LowLevel hdfLowLevel;
	const string mFileName;
	const bool mReadOnly;
};

#endif
