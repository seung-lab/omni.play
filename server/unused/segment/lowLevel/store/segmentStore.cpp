#include "segment/lowLevel/store/segmentStore.h"
#include "segment/lowLevel/pagingPtrStore.h"
#include "segment/io/segmentPage.hpp"

namespace om {
namespace segment {

segmentsStore::segmentsStore(volume::segmentation* segmentation)
    : segmentation_(segmentation)
    , segmentPagesPtr_(new pagingPtrStore(segmentation))
    , segmentPages_(segmentPagesPtr_.get())
{}

segmentsStore::~segmentsStore()
{}

uint32_t segmentsStore::NumPages()
{
    zi::guard g(pagesLock_);
    return segmentPages_->NumPages();
}

uint32_t segmentsStore::PageSize()
{
    zi::guard g(pagesLock_);
    return segmentPages_->PageSize();
}

std::vector<page*> segmentsStore::Pages()
{
    zi::guard g(pagesLock_);
    return segmentPages_->Pages();
}

segment segmentsStore::GetSegmentUnsafe(const common::segId value){
    const uint32_t pageSize = PageSize();
    const uint32_t pageNum = value / pageSize;
    page& page = *(Pages()[pageNum]);
    const uint32_t idx = value % pageSize;
    return page[idx];
}

segment segmentsStore::AddSegment(const common::segId value)
{
    zi::guard g(pagesLock_);
    return segmentPages_->AddSegment(value);
}

void segmentsStore::Flush()
{
    zi::guard g(pagesLock_);
    segmentPages_->Flush();
}

/**
 * a segment ptr is invalid if it is NULL, or has an ID of 0
 **/
bool segmentsStore::IsSegmentValid(const common::segId value)
{
    if(!value){
        return false;
    }

    segment seg = GetSegmentUnsafe(value);

    return value == seg.value();
}

} // namespace segment
} // namespace om
