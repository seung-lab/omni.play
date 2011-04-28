#include "common/omString.hpp"
#include "common/omCommon.h"
#include "common/omDebug.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "mesh/omMeshCoord.h"
#include "mesh/omMeshManager.h"
#include "project/omProject.h"
#include "system/omLocalPreferences.hpp"
#include "system/omStateManager.h"
#include "volume/omChannelImpl.h"
#include "chunks/omChunkCoord.h"
#include "volume/omSegmentation.h"

OmDataPath OmDataPaths::getDefaultDatasetName()
{
    return OmDataPath("main");
}

OmDataPath OmDataPaths::getProjectArchiveNameQT()
{
    return OmDataPath("project.qt.dat");
}

OmDataPath OmDataPaths::getSegmentPagePath(const OmID segmentationID,
                                           const quint32 pageNum)
{
    const std::string p =
        str( boost::format("segmentations/segmentation%1%/segment_page%2%")
             % segmentationID
             % pageNum );

    return OmDataPath(p);
}

std::string OmDataPaths::getMeshFileName( const OmMeshCoord & meshCoord )
{
    return str( boost::format("mesh.%1%.dat")
                %meshCoord.DataValue);
}

std::string OmDataPaths::getDirectoryPath(OmSegmentation* seg)
{
    return str( boost::format("segmentations/segmentation%1%/")
                % seg->GetID());
}

std::string OmDataPaths::getDirectoryPath(OmChannelImpl* chan)
{
    return str( boost::format("channels/channel%1%/")
                % chan->GetID());
}

std::string OmDataPaths::Hdf5VolData(const std::string & dirPath,
                                     const int level)
{
    return dirPath
        + om::string::num(level)
        + "/"
        + "volume.dat";
}
