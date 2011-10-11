#include "common/omString.hpp"
#include "common/common.h"
#include "common/debug.h"
#include "datalayer/dataPath.h"
#include "datalayer/dataPaths.h"
#include "mesh/meshCoord.h"
#include "mesh/mesh::manager.h"
#include "project/project.h"
#include "system/omLocalPreferences.hpp"
#include "system/omStateManager.h"
#include "volume/channelImpl.h"
#include "volume/segmentation.h"

dataPath dataPaths::getDefaultDatasetName(){
    return dataPath("main");
}

dataPath dataPaths::getProjectArchiveNameQT(){
    return dataPath("project.qt.dat");
}

dataPath dataPaths::getSegmentPagePath(const common::id segmentationID, const uint32_t pageNum)
{
    const std::string p =
        str( boost::format("segmentations/segmentation%1%/segment_page%2%")
             % segmentationID
             % pageNum );

    return dataPath(p);
}

std::string dataPaths::getMeshFileName(const meshCoord& meshCoord)
{
    return str( boost::format("mesh.%1%.dat")
                % meshCoord.DataValue);
}

std::string dataPaths::getDirectoryPath(channelImpl const*const chan)
{
    return str( boost::format("channels/channel%1%/")
                % chan->GetID());
}

std::string dataPaths::Hdf5VolData(const std::string& dirPath,
                                     const int level)
{
    return dirPath
        + om::string::num(level)
        + "/"
        + "volume.dat";
}
