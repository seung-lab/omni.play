#ifndef OM_HDF_H
#define OM_HDF_H

#include "common/omCommon.h"
#include "datalayer/omDataWrapper.h"
#include "volume/omVolumeTypes.hpp"
#include "datalayer/omDataReader.h"

#include <zi/mutex>

class OmHdf5Impl;
class OmDataPath;

class OmHdf5 : public OmIDataReader {
public:
	static OmHdf5* getHDF5(const QString& fnp, const bool readOnly);

	virtual ~OmHdf5();

	QString getFileNameAndPath();

	//file
	void open();
	void close();
	void create();
	void flush();

	//group
	bool group_exists( const OmDataPath & path );
	void group_delete( const OmDataPath & path );

	//data set
	bool dataset_exists( const OmDataPath & path );

	//image I/O
	Vector3 < int > getChunkedDatasetDims(const OmDataPath & path );
	void allocateChunkedDataset( const OmDataPath &,
						  const Vector3i&,
						  const Vector3i&,
						  const OmVolDataType type);
	OmDataWrapperPtr readChunkNotOnBoundary( const OmDataPath & path, DataBbox dataExtent);
	void dataset_image_write_trim( const OmDataPath &, const DataBbox&,
				       OmDataWrapperPtr data);

	//data set raw
	OmDataWrapperPtr readDataset( const OmDataPath & path, int* size = NULL);
	void writeDataset( const OmDataPath & path, int size, const OmDataWrapperPtr data);
	OmDataWrapperPtr readChunk( const OmDataPath & path, DataBbox dataExtent);
	void writeChunk(const OmDataPath & path, DataBbox dataExtent, OmDataWrapperPtr data);
	Vector3< int > getDatasetDims( const OmDataPath & path );

private:
	OmHdf5(const QString&, const bool readOnly);

	QString m_fileNameAndPath;
	const bool readOnly_;

	zi::Mutex fileLock;
	boost::shared_ptr<OmHdf5Impl> hdf5_;

	friend class OmHdf5Manager;
};

#endif
