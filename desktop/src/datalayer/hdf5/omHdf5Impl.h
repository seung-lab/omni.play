#pragma once

#include "datalayer/hdf5/omHdf5LowLevel.h"
#include "datalayer/omDataWrapper.h"

class OmDataPath;

class OmHdf5Impl
{
 public:
	OmHdf5Impl(const std::string& fileName, const bool readOnly);
	~OmHdf5Impl();

	void flush();

	bool group_exists(const OmDataPath& path);
	void group_delete(const OmDataPath& path);

	bool dataset_exists(const OmDataPath& path);

	Vector3i getChunkedDatasetDims(const OmDataPath& path,
								   const om::common::AffinityGraph aff);
	void allocateChunkedDataset(const OmDataPath&,
				    const Vector3i& ,
				    const Vector3i&,
				    const OmVolDataType);

	OmDataWrapperPtr readDataset(const OmDataPath&, int* = NULL);
	void allocateDataset(const OmDataPath&,
						 int,
						 const OmDataWrapperPtr data);
	void writeDataset(const OmDataPath& path,
					  int size,
					  const OmDataWrapperPtr data);

	OmDataWrapperPtr readChunk(const OmDataPath& path,
							   const om::dataBbox& dataExtent,
							   const om::common::AffinityGraph aff);
	void writeChunk(const OmDataPath& path,
					om::dataBbox dataExtent,
					OmDataWrapperPtr data);
	Vector3i getDatasetDims(const OmDataPath& path );
	OmDataWrapperPtr GetChunkDataType(const OmDataPath& path);

 private:
	om::shared_ptr<OmHdf5LowLevel> hdf_;
	const bool mReadOnly;
	int fileId;
};

