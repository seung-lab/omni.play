#ifndef OM_DUMMY_WRITER_H
#define OM_DUMMY_WRITER_H

#include "common/omCommon.h"
#include "datalayer/omDataWriter.h"

class OmDummyWriter : public OmDataWriter {
public:
	OmDummyWriter( QString fileNameAndPath );
	~OmDummyWriter();

	void open(){};
	void close(){};
	void flush();

	//file
	void create();

	//group
	void group_delete( const OmDataPath & path );

	//image I/O
	void dataset_image_create_tree_overwrite(const OmDataPath &,
						 const Vector3i&,
						 const Vector3i&,
						 const OmAllowedVolumeDataTypes);

	void dataset_image_write_trim(const OmDataPath &, DataBbox*, OmDataWrapperPtr);
	void dataset_write_raw_chunk_data(const OmDataPath &, DataBbox, OmDataWrapperPtr);

	//data set raw
	void dataset_raw_create_tree_overwrite(const OmDataPath &, int, const OmDataWrapperPtr);

private:
	QString mFileNameAndPath;
};

#endif
