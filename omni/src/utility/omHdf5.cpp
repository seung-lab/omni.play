#include "omHdf5.h"

OmHdf5::OmHdf5( QString fileNameAndPath )
{
	m_fileNameAndPath = fileNameAndPath;
	printf("set hdf5 file name to: \"%s\"\n", qPrintable( fileNameAndPath ) );
}

QString OmHdf5::getFileNameAndPath()
{
	return m_fileNameAndPath;
}

string OmHdf5::getFileNameAndPathCstr()
{
	return m_fileNameAndPath.toStdString();
}

void OmHdf5::create()
{
	om_hdf5_file_create( getFileNameAndPathCstr() );
}

bool OmHdf5::group_exists( string name )
{
	return om_hdf5_group_exists(getFileNameAndPathCstr(), name.c_str());
}

void OmHdf5::group_delete( string name )
{
	om_hdf5_group_delete(getFileNameAndPathCstr(), name.c_str());
}

bool OmHdf5::dataset_exists( string name )
{
	return om_hdf5_dataset_exists(getFileNameAndPathCstr(), name.c_str());
}

void OmHdf5::dataset_image_create_tree_overwrite( string & name, Vector3 < int >dataDims, 
						  Vector3 < int >chunkDims, int bytesPerSample) 
{
	om_hdf5_dataset_image_create_tree_overwrite(getFileNameAndPathCstr(), name.c_str(), 
						    dataDims, chunkDims, bytesPerSample );
}

vtkImageData* OmHdf5::dataset_image_read_trim( string name, DataBbox dataExtent, int bytesPerSample)
{
	return om_hdf5_dataset_image_read_trim(getFileNameAndPathCstr(), name.c_str(), dataExtent, bytesPerSample);
}

void OmHdf5::dataset_image_write_trim( string name, DataBbox dataExtent, 
				       int bytesPerSample, vtkImageData *pImageData)
{
	om_hdf5_dataset_image_write_trim(getFileNameAndPathCstr(), name.c_str(), 
					 dataExtent, bytesPerSample, pImageData);
}

void* OmHdf5::dataset_raw_read( string name, int* size)
{
	return om_hdf5_dataset_raw_read(getFileNameAndPathCstr(), name.c_str(), size);
}

void OmHdf5::dataset_raw_create_tree_overwrite( string name, int size, const void* data)
{
	om_hdf5_dataset_raw_create_tree_overwrite(getFileNameAndPathCstr(), name.c_str(), size, data);
}

