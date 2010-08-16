#ifndef OM_VOLUME_IMPORTER_HPP
#define OM_VOLUME_IMPORTER_HPP

#include "common/omCommon.h"
#include "volume/omMipChunkCoord.h"

class OmDataPath;

template <typename T>
class OmVolumeImporter {
public:
	OmVolumeImporter(T*);

	bool import(OmDataPath & dataset);

	std::pair<int,QString> getNextImgToProcess();
	void addToChunkCoords(const OmMipChunkCoord chunk_coord);

private:
	T* vol;

	std::set<OmMipChunkCoord> chunksToCopy;;

	bool areImportFilesImages();
	bool importHDF5(OmDataPath & dataset);

	bool importImageStack();
	void figureOutNumberOfBytesImg();
};

#endif
