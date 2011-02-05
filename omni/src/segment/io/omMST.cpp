#include "datalayer/fs/omFileNames.hpp"
#include "datalayer/fs/omIOnDiskFile.h"
#include "datalayer/fs/omFileQT.hpp"
#include "segment/io/omMST.h"
#include "segment/io/omMSTold.h"
#include "segment/omSegmentCache.h"
#include "utility/omSmartPtr.hpp"
#include "utility/omStringHelpers.h"

OmMST::OmMST(OmSegmentation* segmentation)
    : segmentation_(segmentation)
    , numEdges_(0)
    , userThreshold_(defaultThresholdSize_)
    , edges_(NULL)
{}

std::string OmMST::memMapPath()
{
    const QString volPath = OmFileNames::MakeVolSegmentsPath(segmentation_);
    const QString ret = QString("%1mst.data").arg(volPath);
    return ret.toStdString();
}

void OmMST::Read()
{
    if(!numEdges_){
        printf("no MST found\n");
        return;
    }

    edgesPtr_ = reader_t::Reader(memMapPath());
    edges_ = edgesPtr_->GetPtr();

    const uint64_t expectedSize = numEdges_*sizeof(OmMSTEdge);

    if( expectedSize != edgesPtr_->Size()){
        const QString err =
            QString("mst sizes did not match: file was %1, but expected %2")
            .arg(edgesPtr_->Size())
            .arg(expectedSize);
        const QString is32bit("; is Omni running on a 32-bit OS?");
        throw OmIoException(err + is32bit);
    }

    for(uint32_t i = 0; i < numEdges_; ++i){
        edges_[i].wasJoined = 0; // always zero out
    }
}

void OmMST::create()
{
    assert(numEdges_);

    edgesPtr_ = writer_t::WriterNumElements(memMapPath(),
                                            numEdges_,
                                            om::ZERO_FILL);

    edges_ = edgesPtr_->GetPtr();
}

void OmMST::Import(const std::vector<OmMSTImportEdge>& importEdges)
{
    numEdges_ = importEdges.size();

    create();

    for(uint32_t i = 0; i < numEdges_; ++i){
        edges_[i].number    = i;
        edges_[i].node1ID   = importEdges[i].node1ID;
        edges_[i].node2ID   = importEdges[i].node2ID;
        edges_[i].threshold = importEdges[i].threshold;
        edges_[i].userSplit = 0;
        edges_[i].userJoin  = 0;
        edges_[i].wasJoined = 0; // always zero out
    }

    Flush();
}

void OmMST::convert()
{
    // numEdges_ already set by OmDataArchiveProject

    if(!numEdges_){
        printf("no MST found\n");
        return;
    }

    OmMSTold old(segmentation_);
    old.ReadOld();

    if(!old.IsPopulated()){
        printf("old MST not populated\n");
        return;
    }

    create();

    old.MoveData(edges_, numEdges_);

    Flush();
}

void OmMST::Flush()
{
    if(numEdges_){
        edgesPtr_->Flush();
    }
}

void OmMST::SetUserThreshold(const double t)
{
    zi::rwmutex::write_guard g(thresholdLock_);

    if(qFuzzyCompare(t, userThreshold_)){
        return;
    }
    userThreshold_ = t;

    segmentation_->SegmentCache()->refreshTree();
}
