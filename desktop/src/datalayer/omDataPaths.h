#pragma once

#include "common/common.h"
#include "coordinates/mesh.h"

class OmAffinityChannel;
class OmChannelImpl;
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

  static std::string getDirectoryPath(OmChannelImpl const* const chan);

  static std::string Hdf5VolData(const std::string& dirPath, const int level);
};
