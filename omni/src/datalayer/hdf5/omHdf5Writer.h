#ifndef OM_HDF_READER_WRITER_H
#define OM_HDF_READER_WRITER_H

#include "datalayer/hdf5/omHdf5.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataWriter.h"

class OmHdf5Writer : public OmDataWriter
{
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
	void dataset_image_create_tree_overwrite( const OmDataPath & path, Vector3<int>* dataDims, Vector3<int>* chunkDims, int bytesPerSample );
	void dataset_image_write_trim( const OmDataPath & path, DataBbox* dataExtent, int bytesPerSample, vtkImageData *pImageData);

	//data set raw
	void dataset_raw_create_tree_overwrite( const OmDataPath & path, int size, const void* data);
	void dataset_write_raw_chunk_data( const OmDataPath & path, DataBbox dataExtent, int bytesPerSample, void* imageData);
 private:
	OmHdf5 * hdf5;

};

#endif
