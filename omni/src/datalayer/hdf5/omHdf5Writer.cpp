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

void OmHdf5Writer::dataset_image_create_tree_overwrite( const OmDataPath & path, Vector3<int>* dataDims, Vector3<int>* chunkDims, OmHdf5Type type)
{
	hdf5->dataset_image_create_tree_overwrite(path, dataDims, chunkDims, type);
}

void OmHdf5Writer::dataset_image_write_trim( const OmDataPath & path, DataBbox *dataExtent, OmDataWrapperPtr data)
{
	hdf5->dataset_image_write_trim( path, dataExtent, data);
}

void OmHdf5Writer::dataset_raw_create_tree_overwrite( const OmDataPath & path, int size, const OmDataWrapperPtr data)
{
	hdf5->dataset_raw_create_tree_overwrite( path, size, data);
}

void OmHdf5Writer::dataset_write_raw_chunk_data( const OmDataPath & path, DataBbox dataExtent, OmDataWrapperPtr data)
{
	hdf5->dataset_write_raw_chunk_data( path, dataExtent, data);
}
