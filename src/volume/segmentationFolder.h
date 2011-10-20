#pragma once

class segmentation;

#include "common/common.h"


namespace om {
namespace segmentation {

class folder {
private:
    volume::segmentation *const vol_;

public:
    folder(volume::segmentation* vol);

    std::string GetVolPath();
    std::string MakeVolFolder();

    std::string GetVolSegmentsPathAbs();
    std::string GetVolSegmentsPathAbs(const std::string& subFolder);

    std::string RelativeVolPath();

    std::string GetMeshChunkFolderPath(const double threshold,
                                   const coords::chunkCoord& coord);

    std::string MakeMeshChunkFolderPath(const double threshold,
                                    const coords::chunkCoord& coord);
    std::string GetMeshFolderPath();
    std::string MakeMeshFolderPath();
    std::string GetMeshThresholdFolderPath(const double threshold);
    std::string MakeMeshThresholdFolderPath(const double threshold);
    std::string MeshMetadataFileOld();
    std::string MeshMetadataFilePerThreshold(const double threshold);
    std::string GetChunksFolder();
    std::string GetChunkFolderPath(const coords::chunkCoord& coord);
    std::string MakeChunkFolderPath(const coords::chunkCoord& coord);

    std::string AnnotationFile();
    std::string LongRangeConnectionFile();

    void MakeUserSegmentsFolder();
};

} // namespace segmentation
} // namespace om
