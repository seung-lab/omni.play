#ifndef OM_SEGMENT_LIST_TYPE_PAGE_HPP
#define OM_SEGMENT_LIST_TYPE_PAGE_HPP

#include "common/omCommon.h"
#include "datalayer/fs/omFile.hpp"
#include "datalayer/fs/omFileNames.hpp"
#include "datalayer/fs/omMemMapCompressedFile.hpp"

class OmSegmentListTypePage {
private:
    OmSegmentation *const segmentation_;
    const PageNum pageNum_;
    const uint32_t pageSize_;
    const std::string fnp_;

    boost::shared_ptr<OmIOnDiskFile<uint8_t> > data_;

public:
    OmSegmentListTypePage(OmSegmentation* segmentation, const PageNum pageNum,
                         const uint32_t pageSize)
        : segmentation_(segmentation)
        , pageNum_(pageNum)
        , pageSize_(pageSize)
        , fnp_(path())
    {}

    uint8_t* Create()
    {
        data_ =
            OmMemMapCompressedFile<uint8_t>::CreateNumElements(fnp_, pageSize_);
        return data_->GetPtr();
    }

    uint8_t* Load()
    {
        data_ =
            boost::make_shared<OmMemMapCompressedFile<uint8_t> >(fnp_);
        return data_->GetPtr();
    }

    uint8_t* Import(boost::shared_ptr<uint8_t> data)
    {
        data_ =
            OmMemMapCompressedFile<uint8_t>::CreateFromData(fnp_, data, pageSize_);
        return data_->GetPtr();
    }

    void Flush(){
        data_->Flush();
    }

private:

    std::string path() const {
        return pathQStr().toStdString();
    }

    QString pathQStr() const
    {
        const QString volPath = OmFileNames::MakeVolSegmentsPath(segmentation_);
        const QString fullPath = QString("%1segment_page%2.%3.ver4")
            .arg(volPath)
            .arg(pageNum_)
            .arg("list_types");

        return fullPath;
    }
};

#endif
