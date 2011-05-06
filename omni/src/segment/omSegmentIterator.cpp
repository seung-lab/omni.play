#include "segment/omSegment.h"
#include "segment/omSegmentIterator.h"
#include "segment/omSegments.h"
#include "utility/segmentDataWrapper.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "zi/omUtility.h"

OmSegmentIterator::OmSegmentIterator(OmSegments* cache)
    : segments_(cache)
{}

OmSegmentIterator::OmSegmentIterator(const SegmentationDataWrapper& sdw)
    : segments_(sdw.Segments())
{}

OmSegmentIterator::OmSegmentIterator(const SegmentDataWrapper& sdw)
    : segments_(sdw.Segments())
{}

void OmSegmentIterator::iterOverSegmentID(const OmSegID segID)
{
    segs_.push_back(segments_->GetSegment(segID));
}

void OmSegmentIterator::iterOverSelectedIDs()
{
    FOR_EACH(iter, segments_->GetSelectedSegmentIds()){
        segs_.push_back( segments_->GetSegment( *iter ));
    }
}

void OmSegmentIterator::iterOverEnabledIDs()
{
    FOR_EACH(iter, segments_->GetEnabledSegmentIds()){
        segs_.push_back( segments_->GetSegment( *iter ) );
    }
}

void OmSegmentIterator::iterOverSegmentIDs(const OmSegIDsSet & set)
{
    FOR_EACH(iter, set){
        segs_.push_back( segments_->GetSegment( *iter ) );
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

    const segChildCont_t& children = segRet->GetChildren();

    FOR_EACH(iter, children)
    {
        segs_.push_back(*iter);
    }

    return segRet;
}

OmSegmentIterator & OmSegmentIterator::operator = (const OmSegmentIterator & other)
{
    if (this == &other) {
        return *this;
    }

    segments_ = other.segments_;
    segs_ = other.segs_;

    return *this;
}
