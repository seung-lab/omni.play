#ifndef OM_MDF5_LOW_LEVEL_WRAPPERS_ABSTRACT_H
#define OM_MDF5_LOW_LEVEL_WRAPPERS_ABSTRACT_H

#include "utility/omHdf5LowLevel.h"
#include "utility/omHdf5Path.h"

class OmHdf5LowLevelWrappersAbstract
{
 public:
	virtual ~OmHdf5LowLevelWrappersAbstract(){};

	virtual void open() = 0;
	virtual void close() = 0;

	//file
	virtual void file_create() = 0;
	
	//group
	virtual bool group_exists_with_lock(OmHdf5Path path) = 0;
	virtual void group_delete_with_lock(OmHdf5Path path) = 0;
	
	//data set
	virtual bool dataset_exists_with_lock(OmHdf5Path path) = 0;

	//image I/O
	virtual Vector3 < int > dataset_image_get_dims_with_lock(OmHdf5Path path) = 0;
	virtual void dataset_image_create_tree_overwrite_with_lock(OmHdf5Path path, Vector3<int>* dataDims, Vector3<int>* chunkDims, int bytesPerSample) = 0;
	virtual vtkImageData* dataset_image_read_trim_with_lock(OmHdf5Path path, DataBbox dataExtent, int bytesPerSample) = 0;
	virtual void dataset_image_write_trim_with_lock(OmHdf5Path path, DataBbox* dataExtent, int bytesPerSample, vtkImageData *pImageData) = 0;
	
	//data set raw
	virtual void* dataset_raw_read_with_lock(OmHdf5Path path, int* size = NULL) = 0;
	virtual void dataset_raw_create_with_lock(OmHdf5Path path, int size, const void *data) = 0;
	virtual void dataset_raw_create_tree_overwrite_with_lock(OmHdf5Path path, int size, const void* data) = 0;
};

#endif
