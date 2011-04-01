#include "datalayer/fs/omMemMappedFileQT.hpp"
#include "utility/omFileHelpers.h"
#include "datalayer/fs/omFileNames.hpp"
#include "datalayer/fs/omIOnDiskFile.h"
#include "datalayer/fs/omFileQT.hpp"
#include "segment/io/omMST.h"
#include "segment/io/omMSTold.h"
#include "segment/omSegments.h"
#include "utility/omSmartPtr.hpp"
#include "utility/omStringHelpers.h"

OmMST::OmMST(OmSegmentation* segmentation)
    : segmentation_(segmentation)
    , fnpActual_(filePathActual())
    // , fnpWorking_(filePathWorking())
    , numEdges_(0)
    , userThreshold_(defaultThreshold_)
    , edges_(NULL)
{}

QString OmMST::filePathActual()
{
    return OmFileNames::MakeVolSegmentsPath(segmentation_)
        + "mst.data";
}

// QString OmMST::filePathWorking() {
//     return filePathActual() + ".working";
// }

void OmMST::Read()
{
    if(!numEdges_){
        printf("no MST found\n");
        return;
    }

    // we will mem-map "working" version to avoid changing the actual MST w/o saving
    // std::cout << "backing up MST..." << std::flush;
    // OmFileHelpers::CopyFile(fnpActual_, fnpWorking_);
    // std::cout << "done\n";

    // edgesPtr_ = reader_t::Reader(fnpWorking_.toStdString());
    edgesPtr_ = reader_t::Reader(fnpActual_.toStdString());
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

    // edgesPtr_ = writer_t::WriterNumElements(fnpWorking_.toStdString(),
    //                                         numEdges_,
    //                                         om::ZERO_FILL);

     edgesPtr_ = writer_t::WriterNumElements(fnpActual_.toStdString(),
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

    // writeToActualFile();
}

// void OmMST::writeToActualFile()
// {
//     QFile file(fnpActual_);
//     if(!file.open(QIODevice::ReadWrite)){
//         throw OmIoException("could not open", fnpActual_);
//     }

//     const int64_t numBytes = sizeof(OmMSTEdge) * numEdges_;

//     const int64_t writeSize =
//         file.write(reinterpret_cast<const char*>(edges_), numBytes);

//     if( writeSize != numBytes) {
//         throw OmIoException("write failed for", fnpActual_);
//     }
// }

void OmMST::SetUserThreshold(const double t)
{
    zi::rwmutex::write_guard g(thresholdLock_);

    if(qFuzzyCompare(t, userThreshold_)){
        return;
    }
    userThreshold_ = t;

    segmentation_->Segments()->refreshTree();
}
