#ifndef OM_MDF5_LOW_LEVEL_WRAPPERS_MANUAL_OPEN_CLOSE_H
#define OM_MDF5_LOW_LEVEL_WRAPPERS_MANUAL_OPEN_CLOSE_H

#include "datalayer/hdf5/omHdf5LowLevel.h"
#include "datalayer/omDataWrapper.h"

class OmDataPath;

class OmHdf5LowLevelWrappersManualOpenClose
{
 public:
	OmHdf5LowLevelWrappersManualOpenClose(string fileName, const bool readOnly);
	~OmHdf5LowLevelWrappersManualOpenClose();

	void open();
	void close();
	void flush();

	//file
	void file_create();

	//group
	bool group_exists(const OmDataPath & path);
	void group_delete(const OmDataPath & path);

	//data set
	bool dataset_exists(const OmDataPath & path);

	//image I/O
	Vector3i dataset_image_get_dims(const OmDataPath & path);
	void dataset_image_create_tree_overwrite(const OmDataPath &,
							   const Vector3i& ,
							   const Vector3i&,
							   const OmVolDataType);
	OmDataWrapperPtr dataset_image_read_trim(const OmDataPath & path, DataBbox dataExtent);
	void dataset_image_write_trim(const OmDataPath&,
						const DataBbox&,
						OmDataWrapperPtr data);

	//data set raw
	OmDataWrapperPtr dataset_raw_read(const OmDataPath &, int* = NULL);
	void dataset_raw_create(const OmDataPath &, int, const OmDataWrapperPtr data);
	void dataset_raw_create_tree_overwrite(const OmDataPath & path, int size, const OmDataWrapperPtr data);
	OmDataWrapperPtr dataset_read_raw_chunk_data(const OmDataPath & path, DataBbox dataExtent);
	void dataset_write_raw_chunk_data(const OmDataPath & path, DataBbox dataExtent, OmDataWrapperPtr data);
	Vector3i dataset_get_dims( const OmDataPath & path );

 private:
	OmHdf5LowLevel hdfLowLevel;
	const string mFileName;
	const bool mReadOnly;
	hid_t fileId;
	bool opened;
};

#endif
