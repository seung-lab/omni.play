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

void OmDummyWriter::group_delete( const OmHdf5Path & )
{
	printf("%s: write operation should not have happened...\n", __FUNCTION__);
}

void OmDummyWriter::dataset_image_create_tree_overwrite( const OmHdf5Path &, Vector3<int>*, 
						  Vector3<int>*, int ) 
{
	printf("%s: write operation should not have happened...\n", __FUNCTION__);
}

void OmDummyWriter::dataset_image_write_trim( const OmHdf5Path & , DataBbox *, 
				       int, vtkImageData *)
{
	printf("%s: write operation should not have happened...\n", __FUNCTION__);
}

void OmDummyWriter::dataset_raw_create_tree_overwrite( const OmHdf5Path &, int, const void* )
{
	printf("%s: write operation should not have happened...\n", __FUNCTION__);
}
