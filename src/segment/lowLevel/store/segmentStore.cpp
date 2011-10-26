#include "segment/lowLevel/store/segmentStore.hpp"
#include "segment/lowLevel/omPagingPtrStore.h"

segmentsStore::segmentsStore(segmentation* segmentation)
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

std::vector<segmentPage*> segmentsStore::Pages()
{
    zi::guard g(pagesLock_);
    return segmentPages_->Pages();
}

segment* segmentsStore::GetSegment(const common::segId value){
    return cachedStore_->GetSegment(value);
}

segment* segmentsStore::GetSegmentUnsafe(const common::segId value){
    return cachedStore_->GetSegmentUnsafe(value);
}

segment* segmentsStore::AddSegment(const common::segId value)
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

    segment* seg = GetSegment(value);

    if(!seg){
        return false;
    }

    return value == seg->value();
}
