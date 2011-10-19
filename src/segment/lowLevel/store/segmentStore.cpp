#include "segment/lowLevel/store/segmentStore.hpp"
#include "segment/lowLevel/store/omCacheSegRootIDs.hpp"
#include "segment/lowLevel/store/omCacheSegStore.hpp"
#include "segment/lowLevel/omPagingPtrStore.h"

segmentsStore::segmentsStore(segmentation* segmentation)
    : segmentation_(segmentation)
    , segmentPagesPtr_(new pagingPtrStore(segmentation))
    , segmentPages_(segmentPagesPtr_.get())
{}

segmentsStore::~segmentsStore()
{}

void segmentsStore::StartCaches()
{
    if(cachedStore_)
    {
        std::cout << "not restarting caches\n";
        return;
    }

    cachedStore_.reset(new OmCacheSegStore(this));
    cacheRootIDs_.reset(new OmCacheSegRootIDs(segmentation_, this));
}

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

common::segId segmentsStore::Root(const common::segId segID){
    return cacheRootIDs_->Root(segID);
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
