#include "omDummyWriter.h"
#include "common/omDebug.h"
#include <stdlib.h>

OmDummyWriter::OmDummyWriter( QString fileNameAndPath, const bool autoOpenAndClose )
{
	mFileNameAndPath = fileNameAndPath;
}

OmDummyWriter::~OmDummyWriter()
{
}

void OmDummyWriter::create()
{
	printf("%s: write operation should not have happened...\n", __FUNCTION__);
}

void OmDummyWriter::group_delete( OmHdf5Path path )
{
	printf("%s: write operation should not have happened...\n", __FUNCTION__);
}

void OmDummyWriter::dataset_image_create_tree_overwrite( OmHdf5Path path, Vector3<int>* dataDims, 
						  Vector3<int>* chunkDims, int bytesPerSample ) 
{
	printf("%s: write operation should not have happened...\n", __FUNCTION__);
}

void OmDummyWriter::dataset_image_write_trim( OmHdf5Path path, DataBbox *dataExtent, 
				       int bytesPerSample, vtkImageData *pImageData)
{
	printf("%s: write operation should not have happened...\n", __FUNCTION__);
}

void OmDummyWriter::dataset_raw_create_tree_overwrite( OmHdf5Path path, int size, const void* data)
{
	printf("%s: write operation should not have happened...\n", __FUNCTION__);
}
