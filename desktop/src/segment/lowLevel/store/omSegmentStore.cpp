#include "segment/lowLevel/store/omSegmentStore.hpp"
#include "segment/lowLevel/store/omCacheSegRootIDs.hpp"
#include "segment/lowLevel/store/omCacheSegStore.hpp"
#include "segment/lowLevel/omPagingPtrStore.h"

OmSegmentsStore::OmSegmentsStore(OmSegmentation* segmentation)
    : segmentation_(segmentation)
    , segmentPagesPtr_(new OmPagingPtrStore(segmentation))
    , segmentPages_(segmentPagesPtr_.get())
{}

OmSegmentsStore::~OmSegmentsStore()
{}

void OmSegmentsStore::StartCaches()
{
    if(cachedStore_)
    {
        std::cout << "not restarting caches\n";
        return;
    }

    cachedStore_.reset(new OmCacheSegStore(this));
    cacheRootIDs_.reset(new OmCacheSegRootIDs(segmentation_, this));
}

uint32_t OmSegmentsStore::NumPages()
{
    zi::guard g(pagesLock_);
    return segmentPages_->NumPages();
}

uint32_t OmSegmentsStore::PageSize()
{
    zi::guard g(pagesLock_);
    return segmentPages_->PageSize();
}

std::vector<OmSegmentPage*> OmSegmentsStore::Pages()
{
    zi::guard g(pagesLock_);
    return segmentPages_->Pages();
}

OmSegment* OmSegmentsStore::GetSegment(const om::common::SegID value){
    return cachedStore_->GetSegment(value);
}

OmSegment* OmSegmentsStore::GetSegmentUnsafe(const om::common::SegID value){
    return cachedStore_->GetSegmentUnsafe(value);
}

om::common::SegID OmSegmentsStore::Root(const om::common::SegID segID){
    return cacheRootIDs_->Root(segID);
}

OmSegment* OmSegmentsStore::AddSegment(const om::common::SegID value)
{
    zi::guard g(pagesLock_);
    return segmentPages_->AddSegment(value);
}

void OmSegmentsStore::Flush()
{
    zi::guard g(pagesLock_);
    segmentPages_->Flush();
}

/**
 * a segment ptr is invalid if it is NULL, or has an ID of 0
 **/
bool OmSegmentsStore::IsSegmentValid(const om::common::SegID value)
{
    if(!value){
        return false;
    }

    OmSegment* seg = GetSegment(value);

    if(!seg){
        return false;
    }

    return value == seg->value();
}
