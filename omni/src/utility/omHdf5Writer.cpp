#include "omHdf5Writer.h"
#include "utility/omHdf5Manager.h"
#include "common/omDebug.h"
#include <stdlib.h>

OmHdf5Writer::OmHdf5Writer( QString fileNameAndPath, const bool autoOpenAndClose )
{
	hdf5 = OmHdf5Manager::getOmHdf5File( fileNameAndPath, autoOpenAndClose );
}

OmHdf5Writer::~OmHdf5Writer()
{
}

void OmHdf5Writer::create()
{
	hdf5->create();
}

void OmHdf5Writer::group_delete( OmHdf5Path path )
{
	hdf5->group_delete( path );
}

void OmHdf5Writer::dataset_image_create_tree_overwrite( OmHdf5Path path, Vector3<int>* dataDims, 
						  Vector3<int>* chunkDims, int bytesPerSample ) 
{
	hdf5->dataset_image_create_tree_overwrite( path, dataDims, chunkDims, bytesPerSample);
}

void OmHdf5Writer::dataset_image_write_trim( OmHdf5Path path, DataBbox *dataExtent, 
				       int bytesPerSample, vtkImageData *pImageData)
{
	hdf5->dataset_image_write_trim( path, dataExtent, bytesPerSample, pImageData);
}

void OmHdf5Writer::dataset_raw_create_tree_overwrite( OmHdf5Path path, int size, const void* data)
{
	hdf5->dataset_raw_create_tree_overwrite( path, size, data);
}
