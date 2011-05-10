#pragma once

#include "common/om.hpp"
#include "common/omCommon.h"

class OmAffinityChannel;
class OmChannelImpl;
class OmDataPath;
class OmChunkCoord;
class OmMeshCoord;
class OmMeshManager;
class OmSegmentation;

class OmDataPaths {
public:
	static OmDataPath getDefaultDatasetName();
	static OmDataPath getProjectArchiveNameQT();

	static OmDataPath getSegmentPagePath( const OmID segmentationID,
										  const quint32 pageNum );

	static std::string getMeshFileName(const OmMeshCoord& meshCoord);

	static std::string getDirectoryPath(OmChannelImpl* chan);
	static std::string getDirectoryPath(OmSegmentation* seg);

	static std::string Hdf5VolData(const std::string& dirPath,
								   const int level);
};

