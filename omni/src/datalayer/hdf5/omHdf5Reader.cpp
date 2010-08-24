#include "omHdf5Reader.h"
#include "datalayer/hdf5/omHdf5.h"
#include "datalayer/hdf5/omHdf5Manager.h"
#include "datalayer/omDataWrapper.h"

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

OmDataWrapperPtr OmHdf5Reader::readChunkNotOnBoundary( const OmDataPath & path, DataBbox dataExtent)
{
	return hdf5->readChunkNotOnBoundary( path, dataExtent);
}

OmDataWrapperPtr OmHdf5Reader::readDataset( const OmDataPath & path, int* size)
{
	return hdf5->readDataset( path, size );
}

Vector3 < int > OmHdf5Reader::dataset_image_get_dims( const OmDataPath & path )
{
	return hdf5->dataset_image_get_dims( path );
}

OmDataWrapperPtr OmHdf5Reader::readChunk( const OmDataPath & path, DataBbox dataExtent)
{
	return hdf5->readChunk( path, dataExtent);
}

Vector3< int > OmHdf5Reader::dataset_get_dims( const OmDataPath & path )
{
	return hdf5->dataset_get_dims( path );
}
