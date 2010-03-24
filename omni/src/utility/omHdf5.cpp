#include "omHdf5.h"
#include "common/omDebug.h"
#include <stdlib.h>
#include <QReadLocker>
#include <QWriteLocker>

OmHdf5::OmHdf5( QString fileNameAndPath, const bool autoOpenAndClose, const bool readOnly )
{
	m_fileNameAndPath = fileNameAndPath;
	fileLock = new QReadWriteLock();
	setHDF5fileAsAutoOpenAndClose( autoOpenAndClose, readOnly );
}

OmHdf5::~OmHdf5()
{
	debug ("meshercrash", "deleting %p %s\n", this, qPrintable(m_fileNameAndPath));
	delete hdfLowLevelWrap;
}

// filename and path accessors
QString OmHdf5::getFileNameAndPath()
{
	return m_fileNameAndPath;
}

string OmHdf5::getFileNameAndPathString()
{
	debug ("meshercrash", "%s\n", m_fileNameAndPath.toStdString().c_str());
	return m_fileNameAndPath.toStdString();
}

// set whether HDF file should be kept opened, 
//  or automatically closed, between calls into the file
void OmHdf5::setHDF5fileAsAutoOpenAndClose( const bool autoOpenAndClose, const bool readOnly )
{
	if( autoOpenAndClose ){
		hdfLowLevelWrap = new OmHdf5LowLevelWrappersAutoOpenClose(getFileNameAndPathString(), readOnly);
	} else {
		hdfLowLevelWrap = new OmHdf5LowLevelWrappersManualOpenClose(getFileNameAndPathString(), readOnly);
	}
}

void OmHdf5::create()
{
	QWriteLocker locker(fileLock);
	hdfLowLevelWrap->file_create();
}

void OmHdf5::open()
{
	QWriteLocker locker(fileLock);
	hdfLowLevelWrap->open();
}

void OmHdf5::close()
{
	QWriteLocker locker(fileLock);
	hdfLowLevelWrap->close();
}

bool OmHdf5::group_exists( OmHdf5Path path )
{
	QReadLocker locker(fileLock);
	return hdfLowLevelWrap->group_exists_with_lock( path );
}

void OmHdf5::group_delete( OmHdf5Path path )
{
	QWriteLocker locker(fileLock);
	hdfLowLevelWrap->group_delete_with_lock( path );
}

bool OmHdf5::dataset_exists( OmHdf5Path path )
{
	QReadLocker locker(fileLock);
	return hdfLowLevelWrap->dataset_exists_with_lock( path );
}

void OmHdf5::dataset_image_create_tree_overwrite( OmHdf5Path path, Vector3<int>* dataDims, 
						  Vector3<int>* chunkDims, int bytesPerSample ) 
{
	QWriteLocker locker(fileLock);
	hdfLowLevelWrap->dataset_image_create_tree_overwrite_with_lock( path, dataDims, chunkDims, bytesPerSample);
}

vtkImageData* OmHdf5::dataset_image_read_trim( OmHdf5Path path, DataBbox dataExtent, int bytesPerSample)
{
	QReadLocker locker(fileLock);
	return hdfLowLevelWrap->dataset_image_read_trim_with_lock( path, dataExtent, bytesPerSample);
}

void OmHdf5::dataset_image_write_trim( OmHdf5Path path, DataBbox *dataExtent, 
				       int bytesPerSample, vtkImageData *pImageData)
{
	QWriteLocker locker(fileLock);
	hdfLowLevelWrap->dataset_image_write_trim_with_lock( path, dataExtent, bytesPerSample, pImageData);
}

void* OmHdf5::dataset_raw_read( OmHdf5Path path, int* size)
{
	QReadLocker locker(fileLock);
	return hdfLowLevelWrap->dataset_raw_read_with_lock( path, size);
}

void OmHdf5::dataset_raw_create_tree_overwrite( OmHdf5Path path, int size, const void* data)
{
	if (!size){
		return;
	}
	
	debug ("meshercrash", "%p, %s, %i, %p\n", this, path.getString().c_str(), size, data);
	QWriteLocker locker(fileLock);
	hdfLowLevelWrap->dataset_raw_create_tree_overwrite_with_lock( path, size, data);
}

Vector3 < int > OmHdf5::dataset_image_get_dims( OmHdf5Path path )
{
	QReadLocker locker(fileLock);
	return hdfLowLevelWrap->dataset_image_get_dims_with_lock( path );
}
