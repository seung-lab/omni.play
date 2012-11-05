#pragma once

#include "common/common.h"
#include "datalayer/omIDataWriter.h"

class OmDummyWriter : public om::common::IDataWriter {
public:
	OmDummyWriter(const std::string&){}

	void open(){};
	void close(){};
	void flush(){
		printf("%s: write operation should not have happened...\n",
		       __FUNCTION__);
	}

	//file
	void create(){
		printf("%s: write operation should not have happened...\n",
		       __FUNCTION__);
	}

	//group
	void group_delete(const OmDataPath &){
		printf("%s: write operation should not have happened...\n",
		       __FUNCTION__);
	}

	//image I/O
	void allocateChunkedDataset(const OmDataPath &,
						 const Vector3i&,
						 const Vector3i&,
						 const om::common::DataType){
		printf("%s: write operation should not have happened...\n",
		       __FUNCTION__);
	}

	void writeChunk(const OmDataPath &, DataBbox,
					  OmDataWrapperPtr){
		printf("%s: write operation should not have happened...\n",
		       __FUNCTION__);
	}

	//data set raw
	void writeDataset(const OmDataPath &, int,
					       const OmDataWrapperPtr){
		printf("%s: write operation should not have happened...\n",
		       __FUNCTION__);
	}
};

