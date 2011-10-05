#pragma once

#include "common/om.hpp"
#include "common/common.h"

class affinityChannel;
class channelImpl;
class dataPath;
class OmMeshCoord;
class OmMeshManager;
class segmentation;

class dataPaths {
public:
    static dataPath getDefaultDatasetName();
    static dataPath getProjectArchiveNameQT();

    static dataPath getSegmentPagePath(const OmID segmentationID,
                                         const uint32_t pageNum );

    static std::string getMeshFileName(const OmMeshCoord& meshCoord);

    static std::string getDirectoryPath(channelImpl const*const chan);

    static std::string Hdf5VolData(const std::string& dirPath,
                                   const int level);
};

