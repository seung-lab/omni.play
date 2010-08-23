#ifndef OM_DUMMY_WRITER_H
#define OM_DUMMY_WRITER_H

#include "common/omCommon.h"
#include "datalayer/omDataWriter.h"

class OmDummyWriter : public OmDataWriter {
public:
	OmDummyWriter(QString){}

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
	void dataset_image_create_tree_overwrite(const OmDataPath &,
						 const Vector3i&,
						 const Vector3i&,
						 const OmVolDataType){
		printf("%s: write operation should not have happened...\n",
		       __FUNCTION__);
	}

	void dataset_write_raw_chunk_data(const OmDataPath &, DataBbox,
					  OmDataWrapperPtr){
		printf("%s: write operation should not have happened...\n",
		       __FUNCTION__);
	}

	//data set raw
	void dataset_raw_create_tree_overwrite(const OmDataPath &, int,
					       const OmDataWrapperPtr){
		printf("%s: write operation should not have happened...\n",
		       __FUNCTION__);
	}
};

#endif
