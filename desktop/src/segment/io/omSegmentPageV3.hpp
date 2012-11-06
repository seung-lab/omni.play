#pragma once

#include "datalayer/fs/omFileNames.hpp"
#include "datalayer/fs/omMemMappedFileQTNew.hpp"
#include "segment/omSegmentTypes.h"

class OmSegmentPageV3 {
private:
    OmSegmentation *const vol_;
    const om::common::PageNum pageNum_;
    const uint32_t pageSize_;
    const std::string fnp_;

    boost::shared_ptr<OmIOnDiskFile<OmSegmentDataV3> > segmentsDataPtr_;

public:
    OmSegmentPageV3(OmSegmentation* vol, const om::common::PageNum pageNum, const uint32_t pageSize)
        : vol_(vol)
        , pageNum_(pageNum)
        , pageSize_(pageSize)
        , fnp_(path())
    {}

    OmSegmentDataV3* Create()
    {
        segmentsDataPtr_ =
            OmMemMappedFileQTNew<OmSegmentDataV3>::CreateNumElements(fnp_, pageSize_);
        return segmentsDataPtr_->GetPtr();
    }

    OmSegmentDataV3* Load()
    {
        segmentsDataPtr_ =
            boost::make_shared<OmMemMappedFileQTNew<OmSegmentDataV3> >(fnp_);
        return segmentsDataPtr_->GetPtr();
    }

    OmSegmentDataV3* Import(boost::shared_ptr<OmSegmentDataV3> data)
    {
        segmentsDataPtr_ =
            OmMemMappedFileQTNew<OmSegmentDataV3>::CreateFromData(fnp_, data, pageSize_);
        return segmentsDataPtr_->GetPtr();
    }

    boost::shared_ptr<OmSegmentDataV3> Read() const
    {
        QFile file(memMapPathQStrV3());

        om::file::openFileRO(file);

        return om::file::readAll<OmSegmentDataV3>(file);
    }

    void Flush(){
        segmentsDataPtr_->Flush();
    }

private:

    std::string path() const {
        return memMapPathQStrV3().toStdString();
    }

    QString memMapPathQStrV3() const
    {
        const std::string fname = str( boost::format("segment_page%1%.data.ver3")
                                       % pageNum_);

        return QString::fromStdString(
            vol_->Folder()->GetVolSegmentsPathAbs(fname)
            );
    }
};
