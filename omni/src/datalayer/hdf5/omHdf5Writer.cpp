#include "datalayer/hdf5/omHdf5Writer.h"
#include "datalayer/hdf5/omHdf5.h"
#include "datalayer/hdf5/omHdf5Manager.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataWrapper.h"

OmHdf5Writer::OmHdf5Writer( QString fileNameAndPath )
{
	hdf5 = OmHdf5Manager::getOmHdf5File( fileNameAndPath, false );
}

OmHdf5Writer::~OmHdf5Writer()
{
	close();
}

void OmHdf5Writer::open()
{
	hdf5->open();
}

void OmHdf5Writer::close()
{
	hdf5->close();
}

void OmHdf5Writer::flush()
{
	hdf5->flush();
}

void OmHdf5Writer::create()
{
	hdf5->create();
}

void OmHdf5Writer::group_delete( const OmDataPath & path )
{
	hdf5->group_delete( path );
}

void OmHdf5Writer::allocateChunkedDataset( const OmDataPath & path,
							const Vector3i& dataDims,
							const Vector3i& chunkDims,
							const OmVolDataType type)
{
	hdf5->allocateChunkedDataset(path, dataDims, chunkDims, type);
}

void OmHdf5Writer::writeDataset(const OmDataPath& path,
						     int size,
						     const OmDataWrapperPtr data)
{
	hdf5->writeDataset( path, size, data);
}

void OmHdf5Writer::writeChunk( const OmDataPath & path, DataBbox dataExtent, OmDataWrapperPtr data)
{
	hdf5->writeChunk( path, dataExtent, data);
}
