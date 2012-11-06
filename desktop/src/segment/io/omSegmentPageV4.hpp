#pragma once

#include "datalayer/fs/omFileNames.hpp"
#include "datalayer/fs/omMemMappedFileQTNew.hpp"
#include "segment/omSegmentTypes.h"

class OmSegmentPageV4 {
private:
    OmSegmentation *const vol_;
    const om::common::PageNum pageNum_;
    const uint32_t pageSize_;
    const std::string fnp_;

    boost::shared_ptr<OmIOnDiskFile<OmSegmentDataV4> > data_;

public:
    OmSegmentPageV4(OmSegmentation* vol, const om::common::PageNum pageNum, const uint32_t pageSize)
        : vol_(vol)
        , pageNum_(pageNum)
        , pageSize_(pageSize)
        , fnp_(path())
    {}

    OmSegmentDataV4* Create()
    {
        data_ =
            OmMemMappedFileQTNew<OmSegmentDataV4>::CreateNumElements(fnp_, pageSize_);
        return data_->GetPtr();
    }

    OmSegmentDataV4* Load()
    {
        data_ =
            boost::make_shared<OmMemMappedFileQTNew<OmSegmentDataV4> >(fnp_);
        return data_->GetPtr();
    }

    OmSegmentDataV4* Import(boost::shared_ptr<OmSegmentDataV4> data)
    {
        data_ =
            OmMemMappedFileQTNew<OmSegmentDataV4>::CreateFromData(fnp_, data, pageSize_);
        return data_->GetPtr();
    }

    void Flush(){
        data_->Flush();
    }

private:

    std::string path(){
        return pathQStr().toStdString();
    }

    QString pathQStr() const
    {
        const std::string fname = str( boost::format("segment_page%1%.data.ver4")
                                       % pageNum_);

        return QString::fromStdString(
            vol_->Folder()->GetVolSegmentsPathAbs(fname)
            );
    }
};

