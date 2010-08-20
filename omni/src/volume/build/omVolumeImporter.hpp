#ifndef OM_VOLUME_IMPORTER_HPP
#define OM_VOLUME_IMPORTER_HPP

#include "common/omCommon.h"
#include "volume/omMipChunkCoord.h"
#include "volume/omVolumeTypes.hpp"

class OmDataPath;

template <typename VOL>
class OmVolumeImporter {
public:
	OmVolumeImporter(VOL*);

	bool import(OmDataPath & dataset);

	void addToChunkCoords(const OmMipChunkCoord chunk_coord);

private:
	VOL *const vol_;

	std::set<OmMipChunkCoord> chunksToCopy;;

	bool areImportFilesImages();
	bool importHDF5(OmDataPath & dataset);

	bool importImageStack();

	OmAllowedVolumeDataTypes figureOutDataType(OmDataPath& path);
	OmAllowedVolumeDataTypes figureOutDataTypeImage();
	OmAllowedVolumeDataTypes figureOutDataTypeHDF5(OmDataPath &);

	void allocateData(const OmAllowedVolumeDataTypes);
	void allocateHDF5(const std::map<int, Vector3i> &);
	void allocateMemMap(const std::map<int, Vector3i> &);
};

#endif
