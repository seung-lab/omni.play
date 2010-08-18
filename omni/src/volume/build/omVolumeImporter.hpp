#ifndef OM_VOLUME_IMPORTER_HPP
#define OM_VOLUME_IMPORTER_HPP

#include "common/omCommon.h"
#include "volume/omMipChunkCoord.h"

class OmDataPath;

template <typename VOL>
class OmVolumeImporter {
public:
	OmVolumeImporter(VOL*);

	bool import(OmDataPath & dataset);

	void addToChunkCoords(const OmMipChunkCoord chunk_coord);

private:
	VOL* vol;

	std::set<OmMipChunkCoord> chunksToCopy;;

	bool areImportFilesImages();
	bool importHDF5(OmDataPath & dataset);

	bool importImageStack();
	OmAllowedVolumeDataTypes figureOutDataType();
};

#endif
