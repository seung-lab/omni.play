#ifndef OM_HDF5_IMPL_H
#define OM_HDF5_IMPL_H

#include "datalayer/hdf5/omHdf5LowLevel.h"
#include "datalayer/omDataWrapper.h"

class OmDataPath;

class OmHdf5Impl
{
 public:
	OmHdf5Impl(string fileName, const bool readOnly);
	~OmHdf5Impl();

	void flush();

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
	boost::shared_ptr<OmHdf5LowLevel> hdf_;
	const bool mReadOnly;
	hid_t fileId;
};

#endif
