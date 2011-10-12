#include "common/omString.hpp"
#include "common/common.h"
#include "common/debug.h"
#include "datalayer/dataPath.h"
#include "datalayer/dataPaths.h"
#include "mesh/meshCoord.h"
#include "mesh/meshManager.h"
#include "project/project.h"
#include "system/localPreferences.hpp"
#include "system/stateManager.h"
#include "volume/channelImpl.h"
#include "volume/segmentation.h"

namespace om {
namespace datalayer {

std::string paths::getDefaultDatasetName(){
    return "main";
}

std::string paths::getProjectArchiveNameQT(){
    return "project.qt.dat";
}

std::string paths::getSegmentPagePath(const common::id segmentationID, const uint32_t pageNum)
{
    const std::string p =
        str( boost::format("segmentations/segmentation%1%/segment_page%2%")
             % segmentationID
             % pageNum );

    return p;
}

std::string paths::getMeshFileName(const mesh::coord& meshCoord)
{
    return str( boost::format("mesh.%1%.dat")
                % meshCoord.DataValue);
}

std::string paths::getDirectoryPath(channelImpl const*const chan)
{
    return str( boost::format("channels/channel%1%/")
                % chan->GetID());
}

std::string paths::Hdf5VolData(const std::string& dirPath,
                                     const int level)
{
    return dirPath
        + om::string::num(level)
        + "/"
        + "volume.dat";
}

} //namespace datalayer
} //namespace om
