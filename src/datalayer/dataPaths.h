#pragma once

#include "common/common.h"

class affinityChannel;
class channelImpl;
class dataPath;
class meshCoord;
class mesh::manager;
class segmentation;

class dataPaths {
public:
    static dataPath getDefaultDatasetName();
    static dataPath getProjectArchiveNameQT();

    static dataPath getSegmentPagePath(const common::id segmentationID,
                                         const uint32_t pageNum );

    static std::string getMeshFileName(const meshCoord& meshCoord);

    static std::string getDirectoryPath(channelImpl const*const chan);

    static std::string Hdf5VolData(const std::string& dirPath,
                                   const int level);
};

