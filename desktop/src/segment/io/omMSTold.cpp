#include "datalayer/hdf5/omHdf5.h"
#include "datalayer/hdf5/omHdf5Manager.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataWrapper.h"
#include "project/omProject.h"
#include "segment/io/omMSTold.h"
#include "segment/io/omMSTtypes.h"
#include "volume/omSegmentation.h"
#include "volume/omSegmentationFolder.h"

OmDataPath OmMSTold::getDendPath() {
    return OmDataPath(segmentation_->Folder()->RelativeVolPath() + "dend");
}

OmDataPath OmMSTold::getDendValuesPath() {
    return OmDataPath(segmentation_->Folder()->RelativeVolPath() + "dendValues");
}

OmDataPath OmMSTold::getEdgeDisabledByUserPath(){
    return OmDataPath(segmentation_->Folder()->RelativeVolPath() + "/edgeDisabledByUser");
};

OmDataPath OmMSTold::getEdgeForceJoinPath(){
    return OmDataPath(segmentation_->Folder()->RelativeVolPath() + "/edgeForceJoin");
};

void OmMSTold::ReadOld()
{
    OmDataPath path(getDendPath());

    OmHdf5* reader = OmProject::OldHDF5();

    if(!reader->dataset_exists(path)) {
        throw om::IoException("did not find MST at path");
    }

    mDend = reader->readDataset(path);

    mDendValues = reader->readDataset(getDendValuesPath());

    mEdgeDisabledByUser = reader->readDataset(getEdgeDisabledByUserPath());

    mEdgeForceJoin = reader->readDataset(getEdgeForceJoinPath());
}

bool OmMSTold::IsPopulated()
{
    if(!mDend ||
       !mDendValues ||
       !mEdgeDisabledByUser ||
       !mEdgeForceJoin)
    {
        return false;
    }

    return true;
}

void OmMSTold::MoveData(OmMSTEdge* edges, const uint32_t numEdges)
{
    const uint32_t* nodes = mDend->getPtr<uint32_t>();
    const float* thresholds = mDendValues->getPtr<float>();
    uint8_t* edgeDisabledByUser = mEdgeDisabledByUser->getPtr<uint8_t>();
    uint8_t* edgeForceJoin = mEdgeForceJoin->getPtr<uint8_t>();

    for(uint32_t i = 0; i < numEdges; ++i){
        edges[i].number    = i;
        edges[i].node1ID   = nodes[i];
        edges[i].node2ID   = nodes[i + numEdges];
        edges[i].threshold = thresholds[i];
        edges[i].userSplit = edgeDisabledByUser[i];
        edges[i].userJoin  = edgeForceJoin[i];
        edges[i].wasJoined = 0; // always zero out
    }
}
