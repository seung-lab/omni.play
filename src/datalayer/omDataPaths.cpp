#include "common/omString.hpp"
#include "common/common.h"
#include "common/omDebug.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "mesh/omMeshCoord.h"
#include "mesh/omMeshManager.h"
#include "project/project.h"
#include "system/omLocalPreferences.hpp"
#include "system/omStateManager.h"
#include "volume/channelImpl.h"
#include "volume/segmentation.h"

OmDataPath OmDataPaths::getDefaultDatasetName(){
    return OmDataPath("main");
}

OmDataPath OmDataPaths::getProjectArchiveNameQT(){
    return OmDataPath("project.qt.dat");
}

OmDataPath OmDataPaths::getSegmentPagePath(const OmID segmentationID, const uint32_t pageNum)
{
    const std::string p =
        str( boost::format("segmentations/segmentation%1%/segment_page%2%")
             % segmentationID
             % pageNum );

    return OmDataPath(p);
}

std::string OmDataPaths::getMeshFileName(const OmMeshCoord& meshCoord)
{
    return str( boost::format("mesh.%1%.dat")
                % meshCoord.DataValue);
}

std::string OmDataPaths::getDirectoryPath(channelImpl const*const chan)
{
    return str( boost::format("channels/channel%1%/")
                % chan->GetID());
}

std::string OmDataPaths::Hdf5VolData(const std::string& dirPath,
                                     const int level)
{
    return dirPath
        + om::string::num(level)
        + "/"
        + "volume.dat";
}
