#ifndef OM_DATA_WRITER_H
#define OM_DATA_WRITER_H

#include "datalayer/omDataWrapper.h"
#include "common/omCommon.h"
#include "volume/omVolumeTypes.hpp"

class vtkImageData;
class OmDataPath;

class OmDataWriter {
public:
	OmDataWriter(){}
	virtual ~OmDataWriter(){}

	virtual void create() = 0;

	virtual void open() = 0;
	virtual void close() = 0;
	virtual void flush() = 0;

	//group
	virtual void group_delete( const OmDataPath & path ) = 0;

	//image I/O
	virtual void dataset_image_create_tree_overwrite( const OmDataPath &,
							  const Vector3i&,
							  const Vector3i&,
							  const OmVolDataType) = 0;

	virtual void dataset_image_write_trim(const OmDataPath&,
					      const DataBbox&,
					      OmDataWrapperPtr) = 0;

	virtual void dataset_write_raw_chunk_data(const OmDataPath &,
						  DataBbox,
						  OmDataWrapperPtr) = 0;

	//data set raw
	virtual void dataset_raw_create_tree_overwrite(const OmDataPath&,
						       int,
						       const OmDataWrapperPtr) = 0;
};

#endif
