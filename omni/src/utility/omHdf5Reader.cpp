#include "omHdf5Reader.h"
#include "utility/omHdf5Manager.h"
#include "common/omDebug.h"
#include <stdlib.h>

OmHdf5Reader::OmHdf5Reader( QString fileNameAndPath, const bool autoOpenAndClose, const bool readOnly )
{
	hdf5 = OmHdf5Manager::getOmHdf5File( fileNameAndPath, autoOpenAndClose, readOnly );
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

bool OmHdf5Reader::group_exists( OmHdf5Path path )
{
	return hdf5->group_exists( path );
}

bool OmHdf5Reader::dataset_exists( OmHdf5Path path )
{
	return hdf5->dataset_exists( path );
}

vtkImageData* OmHdf5Reader::dataset_image_read_trim( OmHdf5Path path, DataBbox dataExtent, int bytesPerSample)
{
	return hdf5->dataset_image_read_trim( path, dataExtent, bytesPerSample);
}

void* OmHdf5Reader::dataset_raw_read( OmHdf5Path path, int* size)
{
	return hdf5->dataset_raw_read( path, size );
}

Vector3 < int > OmHdf5Reader::dataset_image_get_dims( OmHdf5Path path )
{
	return hdf5->dataset_image_get_dims( path );
}
