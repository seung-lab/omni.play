#ifndef OM_DUMMY_WRITER_H
#define OM_DUMMY_WRITER_H

#include "utility/omDataWriter.h"

class OmDummyWriter : public OmDataWriter
{
 public:
	OmDummyWriter( QString fileNameAndPath, const bool autoOpenAndClose );
	~OmDummyWriter();

	//file
	void create();

	//group
	void group_delete( OmHdf5Path path );

	//image I/O
	void dataset_image_create_tree_overwrite( OmHdf5Path path, Vector3<int>* dataDims, Vector3<int>* chunkDims, int bytesPerSample );
	void dataset_image_write_trim( OmHdf5Path path, DataBbox* dataExtent, int bytesPerSample, vtkImageData *pImageData);

	//data set raw
	void dataset_raw_create_tree_overwrite( OmHdf5Path path, int size, const void* data);

 private:
	QString mFileNameAndPath;
};

#endif
