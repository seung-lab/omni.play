#ifndef OM_DATA_READER_H
#define OM_DATA_READER_H

#include "datalayer/omHdf5Path.h"
class vtkImageData;

class OmDataReader
{
 public:
	//paths
	virtual std::string getFileNameAndPathString() = 0;
	virtual QString getFileNameAndPath() = 0;

	//file
	virtual void open() = 0;
	virtual void close() = 0;

	//group
	virtual bool group_exists( const OmHdf5Path & path ) = 0;

	//data set
	virtual bool dataset_exists( const OmHdf5Path & path ) = 0;

	//image I/O
	virtual Vector3 < int > dataset_image_get_dims(const OmHdf5Path & path ) = 0;
	virtual vtkImageData* dataset_image_read_trim( const OmHdf5Path & path, DataBbox dataExtent, int bytesPerSample) = 0;
	virtual void* dataset_read_raw_chunk_data( const OmHdf5Path & path, DataBbox dataExtent, int bytesPerSample)=0;

	//data set raw
	virtual void* dataset_raw_read( const OmHdf5Path & path, int* size = NULL) = 0;

	virtual Vector3< int > dataset_get_dims(const OmHdf5Path & path) = 0;
 };

#endif
