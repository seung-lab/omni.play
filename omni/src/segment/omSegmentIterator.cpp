#include "zi/omUtility.h"
#include "segment/omSegmentIterator.h"
#include "segment/omSegment.h"
#include "segment/omSegments.h"

OmSegmentIterator::OmSegmentIterator(OmSegments* cache)
    : mCache(cache)
{
}

void OmSegmentIterator::iterOverSegmentID(const OmSegID segID)
{
    segs_.push_back(mCache->GetSegment(segID));
}

void OmSegmentIterator::iterOverSelectedIDs()
{
    FOR_EACH(iter, mCache->GetSelectedSegmentIds()){
        segs_.push_back( mCache->GetSegment( *iter ));
    }
}

void OmSegmentIterator::iterOverEnabledIDs()
{
    FOR_EACH(iter, mCache->GetEnabledSegmentIds()){
        segs_.push_back( mCache->GetSegment( *iter ) );
    }
}

void OmSegmentIterator::iterOverSegmentIDs(const OmSegIDsSet & set)
{
    FOR_EACH(iter, set){
        segs_.push_back( mCache->GetSegment( *iter ) );
    }
}

bool OmSegmentIterator::empty()
{
    return segs_.empty();
}

OmSegment* OmSegmentIterator::getNextSegment()
{
    if( segs_.empty() ){
        return NULL;
    }

    OmSegment* segRet = segs_.back();
    segs_.pop_back();

    FOR_EACH(iter, segRet->GetChildren() ){
        segs_.push_back(*iter);
    }

    return segRet;
}

OmSegmentIterator & OmSegmentIterator::operator = (const OmSegmentIterator & other)
{
    if (this == &other) {
        return *this;
    }

    mCache = other.mCache;
    segs_ = other.segs_;

    return *this;
}
