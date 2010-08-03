#ifndef OM_DATA_READER_WRITER_H
#define OM_DATA_READER_WRITER_H

#include "datalayer/omDataWrapper.h"

class vtkImageData;
class OmDataPath;

class OmDataWriter
{
 public:
	virtual void create() = 0;

	virtual void open() = 0;
	virtual void close() = 0;
	virtual void flush() = 0;

	//group
	virtual void group_delete( const OmDataPath & path ) = 0;

	//image I/O
	virtual void dataset_image_create_tree_overwrite( const OmDataPath & path, Vector3<int>* dataDims, Vector3<int>* chunkDims, OmHdf5Type type) = 0;
	virtual void dataset_image_write_trim( const OmDataPath & path, DataBbox* dataExtent, OmDataWrapperPtr data) = 0;

	virtual void dataset_write_raw_chunk_data(const OmDataPath & path, DataBbox dataExtent, OmDataWrapperPtr data) = 0;

	//data set raw
	virtual void dataset_raw_create_tree_overwrite( const OmDataPath & path, int size, const OmDataWrapperPtr data) = 0;
};

#endif
