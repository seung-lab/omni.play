#ifndef OM_DATA_READER_H
#define OM_DATA_READER_H

#include "common/omCommon.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataWrapper.h"

class OmIDataReader {
public:
	virtual ~OmIDataReader(){}

	virtual const std::string& getFileNameAndPath() = 0;

	//file
	virtual void open() = 0;
	virtual void close() = 0;

	virtual bool group_exists( const OmDataPath & path ) = 0;

	virtual bool dataset_exists( const OmDataPath & path ) = 0;

	virtual OmDataWrapperPtr readDataset( const OmDataPath & path, int* size = NULL) = 0;
	virtual Vector3i getDatasetDims(const OmDataPath & path) = 0;

	virtual Vector3i getChunkedDatasetDims(const OmDataPath & path ) = 0;
	virtual OmDataWrapperPtr readChunk( const OmDataPath & path, DataBbox dataExtent)=0;
};
#endif
