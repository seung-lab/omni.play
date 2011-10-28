#pragma once

#include "datalayer/fs/fileNames.hpp"
#include "datalayer/fs/memMappedFile.hpp"
#include "segment/segmentTypes.h"
#include "segment/segment.h"

namespace om {
namespace segment {

class page {
private:
    volume::segmentation *const vol_;
    const common::pageNum pageNum_;
    const uint32_t pageSize_;
    const std::string fnp_;

    boost::shared_ptr<datalayer::IOnDiskFile<data> > data_;

public:
    page(volume::segmentation* vol, const common::pageNum pageNum,
                  const uint32_t pageSize)
        : vol_(vol)
        , pageNum_(pageNum)
        , pageSize_(pageSize)
        , fnp_(path())
    {}

    data* Create()
    {
        data_ = datalayer::memMappedFile<data>::CreateNumElements(fnp_, pageSize_);
        return data_->GetPtr();
    }

    data* Load()
    {
        data_ = boost::make_shared<datalayer::memMappedFile<data> >(fnp_);
        return data_->GetPtr();
    }

    data* Import(boost::shared_ptr<data> d)
    {
        data_ = datalayer::memMappedFile<data>::CreateFromData(fnp_, d, pageSize_);
        return data_->GetPtr();
    }

    void Flush(){
        data_->Flush();
    }

    inline segment operator[](const uint32_t index) {
        return segment(data_->GetPtr() + index, vol_->Segments());
    }

    inline const segment operator[](const uint32_t index) const {
        return segment(data_->GetPtr() + index, vol_->Segments());
    }

private:
    std::string path(){
        const std::string fname = str( boost::format("segment_page%1%.data.ver4")
                                       % pageNum_);
        return vol_->Folder()->GetVolSegmentsPathAbs(fname);
    }
};

} // namespace segment
} // namespace om
