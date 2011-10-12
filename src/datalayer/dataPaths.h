#pragma once

#include "common/common.h"

namespace om {

namespace volume {
class channelImpl;
class segmentation;
}

namespace mesh {
class coord;
class manager;
}

namespace datalayer {

class paths {
public:
    static std::string getDefaultDatasetName();
    static std::string getProjectArchiveNameQT();

    static std::string getSegmentPagePath(const common::id segmentationID,
                                         const uint32_t pageNum );

    static std::string getMeshFileName(const mesh::coord& meshCoord);

    static std::string getDirectoryPath(volume::channelImpl const*const chan);

    static std::string Hdf5VolData(const std::string& dirPath,
                                   const int level);
};

}
}
