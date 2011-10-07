#pragma once

#include "common/common.h"
#include "datalayer/fs/file.h"
#include "datalayer/fs/fileNames.hpp"
#include "datalayer/fs/omMemMapCompressedFile.hpp"

class segmentListTypePage {
private:
    segmentation *const vol_;
    const PageNum pageNum_;
    const uint32_t pageSize_;
    const std::string fnp_;

    om::shared_ptr<OmIOnDiskFile<uint8_t> > data_;

public:
    segmentListTypePage(segmentation* vol, const PageNum pageNum, const uint32_t pageSize)
        : vol_(vol)
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
            om::make_shared<OmMemMapCompressedFile<uint8_t> >(fnp_);
        return data_->GetPtr();
    }

    uint8_t* Import(om::shared_ptr<uint8_t> data)
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

    std::string pathQStr() const
    {
        const std::string fname = str( boost::format("segment_page%1%.list_types.ver4")
                                       % pageNum_);

        return std::string::fromStdString(
            vol_->Folder()->GetVolSegmentsPathAbs(fname)
            );
    }
};

