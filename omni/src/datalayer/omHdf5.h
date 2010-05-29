#ifndef OM_HDF_H
#define OM_HDF_H

#include <QMutex>

#include "common/omCommon.h"
#include "datalayer/omHdf5Helpers.h"
#include "datalayer/omHdf5Path.h"

class OmHdf5LowLevelWrappersManualOpenClose;
class vtkImageData;

class OmHdf5 
{
 public:
	OmHdf5( QString fileNameAndPath, const bool readOnly);
	~OmHdf5();

	string getFileNameAndPathString();
	QString getFileNameAndPath();

	//file
	void open();
	void close();
	void create();
	void flush();

	//group
	bool group_exists( const OmHdf5Path & path );
	void group_delete( const OmHdf5Path & path );

	//data set
	bool dataset_exists( const OmHdf5Path & path );

	//image I/O
	Vector3 < int > dataset_image_get_dims(const OmHdf5Path & path );
	void dataset_image_create_tree_overwrite( const OmHdf5Path & path, Vector3<int>* dataDims, Vector3<int>* chunkDims, int bytesPerSample );
	vtkImageData* dataset_image_read_trim( const OmHdf5Path & path, DataBbox dataExtent, int bytesPerSample);
	void dataset_image_write_trim( const OmHdf5Path & path, DataBbox* dataExtent, int bytesPerSample, vtkImageData *pImageData);

	//data set raw
	void* dataset_raw_read( const OmHdf5Path & path, int* size = NULL);
	void dataset_raw_create_tree_overwrite( const OmHdf5Path & path, int size, const void* data);
	void* dataset_read_raw_chunk_data( const OmHdf5Path & path, DataBbox dataExtent, int bytesPerSample);
	void dataset_write_raw_chunk_data(const OmHdf5Path & path, DataBbox dataExtent, int bytesPerSample, void * imageData);
	Vector3< int > dataset_get_dims( const OmHdf5Path & path );

 private:
	QString m_fileNameAndPath;
	QMutex fileLock;
	OmHdf5LowLevelWrappersManualOpenClose * hdfLowLevelWrap;
};

#endif
