#ifndef OM_DATA_READER_H
#define OM_DATA_READER_H

#include "common/omCommon.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataWrapper.h"

class vtkImageData;

class OmDataReader
{
 public:
	//paths
	virtual QString getFileNameAndPath() = 0;

	//file
	virtual void open() = 0;
	virtual void close() = 0;

	//group
	virtual bool group_exists( const OmDataPath & path ) = 0;

	//data set
	virtual bool dataset_exists( const OmDataPath & path ) = 0;

	//image I/O
	virtual Vector3 < int > getChunkedDatasetDims(const OmDataPath & path ) = 0;
	virtual OmDataWrapperPtr readChunkNotOnBoundary( const OmDataPath & path, DataBbox dataExtent) = 0;
	virtual OmDataWrapperPtr readChunk( const OmDataPath & path, DataBbox dataExtent)=0;

	//data set raw
	virtual OmDataWrapperPtr readDataset( const OmDataPath & path, int* size = NULL) = 0;

	virtual Vector3< int > getDatasetDims(const OmDataPath & path) = 0;
 };

#endif
