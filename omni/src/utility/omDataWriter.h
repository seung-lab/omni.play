#ifndef OM_DATA_READER_WRITER_H
#define OM_DATA_READER_WRITER_H

#include "volume/omVolumeTypes.h"
#include "utility/omHdf5Path.h"

#include <vmmlib/vmmlib.h>
using namespace vmml;

class vtkImageData;

class OmDataWriter
{
 public:

	//file
	virtual void create() = 0;

	//group
	virtual void group_delete( OmHdf5Path path ) = 0;

	//image I/O
	virtual void dataset_image_create_tree_overwrite( OmHdf5Path path, Vector3<int>* dataDims, Vector3<int>* chunkDims, int bytesPerSample ) = 0;
	virtual void dataset_image_write_trim( OmHdf5Path path, DataBbox* dataExtent, int bytesPerSample, vtkImageData *pImageData) = 0;

	//data set raw
	virtual void dataset_raw_create_tree_overwrite( OmHdf5Path path, int size, const void* data) = 0;
};

#endif
