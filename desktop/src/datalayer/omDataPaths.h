#pragma once
#include "precomp.h"

#include "common/common.h"
#include "coordinates/mesh.h"


class OmAffinityChannel;
class OmChannel;
class OmDataPath;
class OmMeshManager;
class OmSegmentation;

class OmDataPaths {
 public:
  static OmDataPath getDefaultDatasetName();
  static OmDataPath getProjectArchiveNameQT();

  static OmDataPath getSegmentPagePath(const om::common::ID segmentationID,
                                       const uint32_t pageNum);

  static std::string getMeshFileName(const om::coords::Mesh& meshCoord);

  static std::string getDirectoryPath(const OmChannel * const chan);

  static std::string Hdf5VolData(const std::string& dirPath, const int level);
};
