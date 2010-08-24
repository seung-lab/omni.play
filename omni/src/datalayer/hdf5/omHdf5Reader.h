#ifndef OM_HDF_READER_H
#define OM_HDF_READER_H

#include <QMutex>

#include "common/omCommon.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataReader.h"
#include "datalayer/omDataWrapper.h"

class vtkImageData;
class OmDataPath;
class OmHdf5;

class OmHdf5Reader : public OmDataReader
{
 public:
	OmHdf5Reader( QString fileNameAndPath, const bool readOnly);
	~OmHdf5Reader();

	QString getFileNameAndPath();

	//file
	void open();
	void close();

	//group
	bool group_exists( const OmDataPath & path );

	//data set
	bool dataset_exists( const OmDataPath & path );

	//image I/O
	Vector3 < int > dataset_image_get_dims(const OmDataPath & path );
	OmDataWrapperPtr readChunkNotOnBoundary( const OmDataPath & path, DataBbox dataExtent);

	//data set raw
	OmDataWrapperPtr readDataset( const OmDataPath & path, int* size = NULL);
	OmDataWrapperPtr readChunk( const OmDataPath & path, DataBbox dataExtent);
	Vector3< int > dataset_get_dims( const OmDataPath & path );

 private:
	OmHdf5 * hdf5;
};

#endif
