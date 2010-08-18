#ifndef OM_HDF_H
#define OM_HDF_H

#include <QMutex>

#include "common/omCommon.h"
#include "datalayer/omDataWrapper.h"

class OmHdf5LowLevelWrappersManualOpenClose;
class vtkImageData;
class OmDataPath;

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
	bool group_exists( const OmDataPath & path );
	void group_delete( const OmDataPath & path );

	//data set
	bool dataset_exists( const OmDataPath & path );

	//image I/O
	Vector3 < int > dataset_image_get_dims(const OmDataPath & path );
	void dataset_image_create_tree_overwrite( const OmDataPath &,
						  const Vector3i&,
						  const Vector3i&,
						  const OmAllowedVolumeDataTypes type);
	OmDataWrapperPtr dataset_image_read_trim( const OmDataPath & path, DataBbox dataExtent);
	void dataset_image_write_trim( const OmDataPath & path, DataBbox* dataExtent, OmDataWrapperPtr data);

	//data set raw
	OmDataWrapperPtr dataset_raw_read( const OmDataPath & path, int* size = NULL);
	void dataset_raw_create_tree_overwrite( const OmDataPath & path, int size, const OmDataWrapperPtr data);
	OmDataWrapperPtr dataset_read_raw_chunk_data( const OmDataPath & path, DataBbox dataExtent);
	void dataset_write_raw_chunk_data(const OmDataPath & path, DataBbox dataExtent, OmDataWrapperPtr data);
	Vector3< int > dataset_get_dims( const OmDataPath & path );

 private:
	QString m_fileNameAndPath;
	mutable QMutex fileLock;
	OmHdf5LowLevelWrappersManualOpenClose * hdfLowLevelWrap;
};

#endif
