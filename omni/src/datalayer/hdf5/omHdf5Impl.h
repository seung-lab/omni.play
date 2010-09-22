#ifndef OM_HDF5_IMPL_H
#define OM_HDF5_IMPL_H

#include "datalayer/hdf5/omHdf5LowLevel.h"
#include "datalayer/omDataWrapper.h"

class OmDataPath;

class OmHdf5Impl
{
 public:
	OmHdf5Impl(const std::string& fileName, const bool readOnly);
	~OmHdf5Impl();

	void flush();

	//group
	bool group_exists(const OmDataPath & path);
	void group_delete(const OmDataPath & path);

	//data set
	bool dataset_exists(const OmDataPath & path);

	//image I/O
	Vector3i getChunkedDatasetDims(const OmDataPath & path);
	void allocateChunkedDataset(const OmDataPath &,
							   const Vector3i& ,
							   const Vector3i&,
							   const OmVolDataType);
	OmDataWrapperPtr readChunkNotOnBoundary(const OmDataPath & path, DataBbox dataExtent);
	void dataset_image_write_trim(const OmDataPath&,
						const DataBbox&,
						OmDataWrapperPtr data);

	//data set raw
	OmDataWrapperPtr readDataset(const OmDataPath &, int* = NULL);
	void allocateDataset(const OmDataPath &, int, const OmDataWrapperPtr data);
	void writeDataset(const OmDataPath & path, int size, const OmDataWrapperPtr data);
	OmDataWrapperPtr readChunk(const OmDataPath & path, DataBbox dataExtent);
	void writeChunk(const OmDataPath & path, DataBbox dataExtent, OmDataWrapperPtr data);
	Vector3i getDatasetDims( const OmDataPath & path );

 private:
	boost::shared_ptr<OmHdf5LowLevel> hdf_;
	const bool mReadOnly;
	int fileId;
};

#endif
