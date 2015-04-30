#include "common/string.hpp"
#include "common/common.h"
#include "common/logging.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "view3d/mesh/omMeshManager.h"
#include "project/omProject.h"
#include "system/omLocalPreferences.hpp"
#include "system/omStateManager.h"
#include "volume/omChannelImpl.h"
#include "volume/omSegmentation.h"

OmDataPath OmDataPaths::getDefaultDatasetName() { return OmDataPath("main"); }

OmDataPath OmDataPaths::getProjectArchiveNameQT() {
  return OmDataPath("project.qt.dat");
}

OmDataPath OmDataPaths::getSegmentPagePath(const om::common::ID segmentationID,
                                           const uint32_t pageNum) {
  const std::string p =
      str(boost::format("segmentations/segmentation%1%/segment_page%2%") %
          segmentationID % pageNum);

  return OmDataPath(p);
}

std::string OmDataPaths::getMeshFileName(const om::coords::Mesh& meshCoord) {
  return str(boost::format("mesh.%1%.dat") % meshCoord.segID());
}

std::string OmDataPaths::getDirectoryPath(OmChannelImpl const* const chan) {
  return str(boost::format("channels/channel%1%/") % chan->GetID());
}

std::string OmDataPaths::Hdf5VolData(const std::string& dirPath,
                                     const int level) {
  return dirPath + om::string::num(level) + "/" + "volume.dat";
}
