#pragma once

class OmSegmentation;

#include "common/common.h"
#include "coordinates/chunk.h"
#include <QString>

namespace om {
namespace segmentation {

class folder {
 private:
  OmSegmentation* const vol_;

 public:
  folder(OmSegmentation* vol);

  QString GetVolPath();
  QString MakeVolFolder();

  std::string GetVolSegmentsPathAbs();
  std::string GetVolSegmentsPathAbs(const std::string& subFolder);

  QString RelativeVolPath();

  QString GetMeshChunkFolderPath(const double threshold,
                                 const om::coords::Chunk& coord);

  QString MakeMeshChunkFolderPath(const double threshold,
                                  const om::coords::Chunk& coord);
  QString GetMeshFolderPath();
  QString MakeMeshFolderPath();
  QString GetMeshThresholdFolderPath(const double threshold);
  QString MakeMeshThresholdFolderPath(const double threshold);
  QString MeshMetadataFileOld();
  QString MeshMetadataFilePerThreshold(const double threshold);
  QString GetChunksFolder();
  QString GetChunkFolderPath(const om::coords::Chunk& coord);
  QString MakeChunkFolderPath(const om::coords::Chunk& coord);

  QString AnnotationFile();
  QString LongRangeConnectionFile();

  void MakeUserSegmentsFolder();
};

}  // namespace segmentation
}  // namespace om
