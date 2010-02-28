#ifndef OM_HDF_H
#define OM_HDF_H

#include <QMutex>
#include <QString>
#include <QQueue>
#include <string>
using std::string;

#include "volume/omVolumeTypes.h"
#include "omHdf5LowLevelWrappersAutoOpenClose.h"
#include "omHdf5LowLevelWrappersManualOpenClose.h"
#include "omHdf5Helpers.h"
#include "omHdf5Dataset.h"

class vtkImageData;

class OmHdf5 
{
 public:
	OmHdf5( QString fileNameAndPath, const bool autoOpenAndClose = true );
	~OmHdf5();

	string getFileNameAndPathString();
	QString getFileNameAndPath();
	void resetHDF5fileAsAutoOpenAndClose( const bool autoOpenAndClose );

	//file
	void open();
	void close();
	void create();

	//group
	bool group_exists( string name );
	void group_delete( string name );

	//data set
	bool dataset_exists( string name );

	//image I/O
	Vector3 < int > dataset_image_get_dims(string name );
	void dataset_image_create_tree_overwrite( string & path, Vector3 < int >dataDims, Vector3 < int >chunkDims, int bytesPerSample, bool unlimited  = false);
	vtkImageData* dataset_image_read_trim( string name, DataBbox dataExtent, int bytesPerSample);
	void dataset_image_write_trim( string name, DataBbox dataExtent, int bytesPerSample, vtkImageData *pImageData);

	//data set raw
	void* dataset_raw_read( string name, int* size = NULL);
	void dataset_raw_create_tree_overwrite( string name, int size, const void* data);

 private:
	QString m_fileNameAndPath;
	QQueue <OmHdf5DataSet*> mQueue;
	QMutex * fileLock;
	OmHdf5LowLevelWrappersAbstract * hdfLowLevelWrap;
	void setHDF5fileAsAutoOpenAndClose( const bool autoOpenAndClose );
};

#endif
