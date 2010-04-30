#ifndef OM_HDF_READER_H
#define OM_HDF_READER_H

#include <QMutex>
#include <QQueue>

#include "common/omCommon.h"
#include "utility/omHdf5.h"
#include "utility/omHdf5Path.h"
#include "utility/omDataReader.h"

class vtkImageData;

class OmHdf5Reader : public OmDataReader
{
 public:
	OmHdf5Reader( QString fileNameAndPath, const bool autoOpenAndClose, const bool readOnly);
	~OmHdf5Reader();

	std::string getFileNameAndPathString();
	QString getFileNameAndPath();

	//file
	void open();
	void close();

	//group
	bool group_exists( OmHdf5Path path );

	//data set
	bool dataset_exists( OmHdf5Path path );

	//image I/O
	Vector3 < int > dataset_image_get_dims(OmHdf5Path path );
	vtkImageData* dataset_image_read_trim( OmHdf5Path path, DataBbox dataExtent, int bytesPerSample);
	void* dataset_image_read_simple_trim( OmHdf5Path path, DataBbox dataExtent);
	//data set raw
	void* dataset_raw_read( OmHdf5Path path, int* size = NULL);
	void* dataset_read_raw_chunk_data( OmHdf5Path path, DataBbox dataExtent, int bytesPerSample);
	Vector3< int > dataset_get_dims( OmHdf5Path path );

 private:
	OmHdf5 * hdf5;
};

#endif
