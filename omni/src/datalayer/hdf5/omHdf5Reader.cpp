#include "omHdf5Reader.h"
#include "datalayer/hdf5/omHdf5Manager.h"
#include "common/omDebug.h"
#include <stdlib.h>

OmHdf5Reader::OmHdf5Reader( QString fileNameAndPath, const bool readOnly )
{
	hdf5 = OmHdf5Manager::getOmHdf5File( fileNameAndPath, readOnly );
}

OmHdf5Reader::~OmHdf5Reader()
{
}

// filename and path accessors
QString OmHdf5Reader::getFileNameAndPath()
{
	return hdf5->getFileNameAndPath();
}

std::string OmHdf5Reader::getFileNameAndPathString()
{
	return hdf5->getFileNameAndPathString();
}

void OmHdf5Reader::open()
{
	hdf5->open();
}

void OmHdf5Reader::close()
{
	hdf5->close();
}

bool OmHdf5Reader::group_exists( const OmDataPath & path )
{
	return hdf5->group_exists( path );
}

bool OmHdf5Reader::dataset_exists( const OmDataPath & path )
{
	return hdf5->dataset_exists( path );
}

vtkImageData* OmHdf5Reader::dataset_image_read_trim( const OmDataPath & path, DataBbox dataExtent, int bytesPerSample)
{
	return hdf5->dataset_image_read_trim( path, dataExtent, bytesPerSample);
}

void* OmHdf5Reader::dataset_raw_read( const OmDataPath & path, int* size)
{
	return hdf5->dataset_raw_read( path, size );
}

Vector3 < int > OmHdf5Reader::dataset_image_get_dims( const OmDataPath & path )
{
	return hdf5->dataset_image_get_dims( path );
}

void* OmHdf5Reader::dataset_read_raw_chunk_data( const OmDataPath & path, DataBbox dataExtent, int bytesPerSample)
{
	return hdf5->dataset_read_raw_chunk_data( path, dataExtent, bytesPerSample );
}

Vector3< int > OmHdf5Reader::dataset_get_dims( const OmDataPath & path )
{
	return hdf5->dataset_get_dims( path );
}
