#ifndef OM_HDF_READER_WRITER_H
#define OM_HDF_READER_WRITER_H

#include "common/omCommon.h"
#include "datalayer/omDataWriter.h"
#include "datalayer/omDataWrapper.h"

class OmHdf5;
class OmDataPath;
class vtkImageData;

class OmHdf5Writer : public OmDataWriter {
public:
	OmHdf5Writer( QString fileNameAndPath);
	~OmHdf5Writer();

	void open();
	void close();
	void flush();

	//file
	void create();

	//group
	void group_delete( const OmDataPath & path );

	//image I/O
	void allocateChunkedDataset(const OmDataPath &,
						 const Vector3i&,
						 const Vector3i&,
						 const OmVolDataType);
	void dataset_image_write_trim( const OmDataPath & ,
				       const DataBbox&, OmDataWrapperPtr data);

	//data set raw
	void writeDataset( const OmDataPath & path, int size, const OmDataWrapperPtr data);
	void writeChunk( const OmDataPath & path, DataBbox dataExtent, OmDataWrapperPtr data);
private:
	OmHdf5 * hdf5;

};

#endif
