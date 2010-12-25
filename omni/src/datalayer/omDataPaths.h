#ifndef OM_DATA_PATHS_H
#define OM_DATA_PATHS_H

#include "common/om.hpp"

class OmChannel;
class OmDataPath;
class OmMipChunkCoord;
class OmMipMeshCoord;
class OmMipMeshManager;
class OmSegmentation;

class OmDataPaths {
public:
	static std::string getDefaultHDF5channelDatasetName(const om::Affinity aff = om::NO_AFFINITY);

	static OmDataPath getDefaultDatasetName();
	static OmDataPath getProjectArchiveNameQT();

	static OmDataPath getSegmentPagePath( const OmID segmentationID,
										  const quint32 pageNum );

	static std::string getMeshFileName(const OmMipMeshCoord& meshCoord);
	static std::string getLocalPathForHd5fChunk(const OmMipMeshCoord& meshCoord,
												const OmID segmentationID);

	static std::string getDirectoryPath(OmChannel* chan);
	static std::string getDirectoryPath(OmSegmentation* seg);

	static std::string MipLevelInternalDataPath(const std::string& dirPath,
												const int level);

	static std::string MipChunkMetaDataPath(const std::string& dirPath,
											const OmMipChunkCoord& coord);
};

#endif
