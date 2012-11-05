#pragma once

#include "common/om.hpp"
#include "common/common.h"

class OmAffinityChannel;
class OmChannelImpl;
class OmDataPath;
class OmMeshCoord;
class OmMeshManager;
class OmSegmentation;

class OmDataPaths {
public:
    static OmDataPath getDefaultDatasetName();
    static OmDataPath getProjectArchiveNameQT();

    static OmDataPath getSegmentPagePath(const om::common::ID segmentationID,
                                         const uint32_t pageNum );

    static std::string getMeshFileName(const OmMeshCoord& meshCoord);

    static std::string getDirectoryPath(OmChannelImpl const*const chan);

    static std::string Hdf5VolData(const std::string& dirPath,
                                   const int level);
};

