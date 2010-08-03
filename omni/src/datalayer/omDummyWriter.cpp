#include "omDummyWriter.h"
#include "common/omDebug.h"
#include <stdlib.h>

OmDummyWriter::OmDummyWriter( QString fileNameAndPath )
{
	mFileNameAndPath = fileNameAndPath;
}

OmDummyWriter::~OmDummyWriter()
{
}

void OmDummyWriter::flush()
{
	printf("%s: write operation should not have happened...\n", __FUNCTION__);
}

void OmDummyWriter::create()
{
	printf("%s: write operation should not have happened...\n", __FUNCTION__);
}

void OmDummyWriter::group_delete( const OmDataPath & )
{
	printf("%s: write operation should not have happened...\n", __FUNCTION__);
}

void OmDummyWriter::dataset_image_create_tree_overwrite( const OmDataPath &, Vector3<int>*, 
						  Vector3<int>*, OmHdf5Type) 
{
	printf("%s: write operation should not have happened...\n", __FUNCTION__);
}

void OmDummyWriter::dataset_image_write_trim( const OmDataPath & , DataBbox *, 
				       OmDataWrapperPtr)
{
	printf("%s: write operation should not have happened...\n", __FUNCTION__);
}

void OmDummyWriter::dataset_raw_create_tree_overwrite( const OmDataPath &, int, OmDataWrapperPtr)
{
	printf("%s: write operation should not have happened...\n", __FUNCTION__);
}

void OmDummyWriter::dataset_write_raw_chunk_data(const OmDataPath &, DataBbox, OmDataWrapperPtr)
{
	printf("%s: write operation should not have happened...\n", __FUNCTION__);
}
