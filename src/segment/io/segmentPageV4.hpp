#pragma once

#include "datalayer/fs/fileNames.hpp"
#include "datalayer/fs/omMemMappedFileQTNew.hpp"
#include "segment/segmentTypes.h"

class segmentPageV4 {
private:
    segmentation *const vol_;
    const PageNum pageNum_;
    const uint32_t pageSize_;
    const std::string fnp_;

    boost::shared_ptr<OmIOnDiskFile<segmentDataV4> > data_;

public:
    segmentPageV4(segmentation* vol, const PageNum pageNum, const uint32_t pageSize)
        : vol_(vol)
        , pageNum_(pageNum)
        , pageSize_(pageSize)
        , fnp_(path())
    {}

    segmentDataV4* Create()
    {
        data_ =
            memMappedFileNew<segmentDataV4>::CreateNumElements(fnp_, pageSize_);
        return data_->GetPtr();
    }

    segmentDataV4* Load()
    {
        data_ =
            boost::make_shared<memMappedFileNew<segmentDataV4> >(fnp_);
        return data_->GetPtr();
    }

    segmentDataV4* Import(boost::shared_ptr<segmentDataV4> data)
    {
        data_ =
            memMappedFileNew<segmentDataV4>::CreateFromData(fnp_, data, pageSize_);
        return data_->GetPtr();
    }

    void Flush(){
        data_->Flush();
    }

private:

    std::string path(){
        return pathQStr().toStdString();
    }

    std::string pathQStr() const
    {
        const std::string fname = str( boost::format("segment_page%1%.data.ver4")
                                       % pageNum_);

        return std::string::fromStdString(
            vol_->Folder()->GetVolSegmentsPathAbs(fname)
            );
    }
};

