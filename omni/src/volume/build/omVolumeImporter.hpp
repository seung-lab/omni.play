#ifndef OM_VOLUME_IMPORTER_HPP
#define OM_VOLUME_IMPORTER_HPP

#include "common/omCommon.h"
#include "volume/omMipChunkCoord.h"
#include "volume/omVolumeTypes.hpp"

class OmDataPath;
class OmDataReader;

template <typename VOL>
class OmVolumeImporter {
public:
	OmVolumeImporter(VOL*);

	bool import(OmDataPath & dataset);

private:
	VOL *const vol_;

	std::set<OmMipChunkCoord> chunksToCopy;;

	bool doImport(OmDataPath& path);
	bool importHDF5(OmDataPath & dataset);
	bool importImageStack();

	bool areImportFilesImages();
	OmVolDataType figureOutDataType(OmDataPath& path);
	OmVolDataType figureOutDataTypeImage();
	OmVolDataType figureOutDataTypeHDF5(OmDataPath &);

	void allocateData(const OmVolDataType);
	void allocateHDF5(const std::map<int, Vector3i> &);
	void allocateMemMap(const std::map<int, Vector3i> &);

	OmDataPath getHDFsrcPath(OmDataReader*, const OmDataPath&);
	QString getHDFfileNameAndPath();
};

#endif
