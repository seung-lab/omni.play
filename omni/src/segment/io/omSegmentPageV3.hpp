#pragma once

#include "datalayer/fs/omFileNames.hpp"
#include "datalayer/fs/omMemMappedFileQTNew.hpp"
#include "segment/omSegmentTypes.h"

class OmSegmentPageV3 {
private:
    OmSegmentation *const segmentation_;
    const PageNum pageNum_;
    const uint32_t pageSize_;
    const std::string fnp_;

    om::shared_ptr<OmIOnDiskFile<OmSegmentDataV3> > segmentsDataPtr_;

public:
    OmSegmentPageV3(OmSegmentation* segmentation, const PageNum pageNum,
                    const uint32_t pageSize)
        : segmentation_(segmentation)
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
            om::make_shared<OmMemMappedFileQTNew<OmSegmentDataV3> >(fnp_);
        return segmentsDataPtr_->GetPtr();
    }

    OmSegmentDataV3* Import(om::shared_ptr<OmSegmentDataV3> data)
    {
        segmentsDataPtr_ =
            OmMemMappedFileQTNew<OmSegmentDataV3>::CreateFromData(fnp_, data, pageSize_);
        return segmentsDataPtr_->GetPtr();
    }

    om::shared_ptr<OmSegmentDataV3> Read() const
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
        const QString volPath = OmFileNames::MakeVolSegmentsPath(segmentation_);
        const QString fullPath = QString("%1segment_page%2.%3.ver3")
            .arg(volPath)
            .arg(pageNum_)
            .arg("data");

        return fullPath;
    }
};

