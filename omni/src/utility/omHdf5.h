#ifndef OM_HDF_H
#define OM_HDF_H

#include <QMutex>
#include <QQueue>
#include <string>
using std::string;

#include "common/omCommon.h"
#include "utility/omHdf5LowLevelWrappersAutoOpenClose.h"
#include "utility/omHdf5LowLevelWrappersManualOpenClose.h"
#include "utility/omHdf5Helpers.h"
#include "utility/omHdf5Dataset.h"
#include "utility/omHdf5Path.h"

class vtkImageData;

class OmHdf5 
{
 public:
	OmHdf5( QString fileNameAndPath, const bool autoOpenAndClose, const bool readOnly);
	~OmHdf5();

	string getFileNameAndPathString();
	QString getFileNameAndPath();

	//file
	void open();
	void close();
	void create();

	//group
	bool group_exists( OmHdf5Path path );
	void group_delete( OmHdf5Path path );

	//data set
	bool dataset_exists( OmHdf5Path path );

	//image I/O
	Vector3 < int > dataset_image_get_dims(OmHdf5Path path );
	void dataset_image_create_tree_overwrite( OmHdf5Path path, Vector3<int>* dataDims, Vector3<int>* chunkDims, int bytesPerSample );
	vtkImageData* dataset_image_read_trim( OmHdf5Path path, DataBbox dataExtent, int bytesPerSample);
	void dataset_image_write_trim( OmHdf5Path path, DataBbox* dataExtent, int bytesPerSample, vtkImageData *pImageData);

	//data set raw
	void* dataset_raw_read( OmHdf5Path path, int* size = NULL);
	void dataset_raw_create_tree_overwrite( OmHdf5Path path, int size, const void* data);
	void* dataset_read_raw_chunk_data( OmHdf5Path path, DataBbox dataExtent, int bytesPerSample);
	void dataset_write_raw_chunk_data(OmHdf5Path path, DataBbox dataExtent, int bytesPerSample, void * imageData);
	Vector3< int > dataset_get_dims( OmHdf5Path path );

 private:
	QString m_fileNameAndPath;
	QQueue <OmHdf5DataSet*> mQueue;
	QMutex * fileLock;
	OmHdf5LowLevelWrappersAbstract * hdfLowLevelWrap;
	void setHDF5fileAsAutoOpenAndClose( const bool autoOpenAndClose, const bool readOnly );
};

#endif
