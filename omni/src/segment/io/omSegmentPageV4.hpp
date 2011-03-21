#ifndef OM_SEGMENT_PAGE_V4_HPP
#define OM_SEGMENT_PAGE_V4_HPP

#include "datalayer/fs/omFileNames.hpp"
#include "datalayer/fs/omMemMappedFileQTNew.hpp"
#include "segment/omSegmentTypes.h"

class OmSegmentPageV4 {
private:
    OmSegmentation *const segmentation_;
    const PageNum pageNum_;
    const uint32_t pageSize_;
    const std::string fnp_;

    boost::shared_ptr<OmIOnDiskFile<OmSegmentDataV4> > data_;

public:
    OmSegmentPageV4(OmSegmentation* segmentation, const PageNum pageNum,
                    const uint32_t pageSize)
        : segmentation_(segmentation)
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
        const QString volPath = OmFileNames::MakeVolSegmentsPath(segmentation_);
        const QString fullPath = QString("%1segment_page%2.%3.ver4")
            .arg(volPath)
            .arg(pageNum_)
            .arg("data");

        return fullPath;
    }
};

#endif
