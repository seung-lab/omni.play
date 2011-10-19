#include "segment/segment.h"
#include "segment/segmentIterator.h"
#include "segment/segments.h"
#include "utility/segmentDataWrapper.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "zi/utility.h"

segmentIterator::segmentIterator(segments* cache)
    : segments_(cache)
{}

segmentIterator::segmentIterator(const SegmentationDataWrapper& sdw)
    : segments_(sdw.Segments())
{}

segmentIterator::segmentIterator(const SegmentDataWrapper& sdw)
    : segments_(sdw.Segments())
{}

void segmentIterator::iterOverSegmentID(const common::segId segID){
    segs_.push_back(segments_->GetSegment(segID));
}

void segmentIterator::iterOverSelectedIDs()
{
    const common::segIdsSet ids = segments_->GetSelectedSegmentIDs();
    FOR_EACH(iter, ids){
        segs_.push_back( segments_->GetSegment( *iter ));
    }
}

void segmentIterator::iterOverEnabledIDs()
{
    const common::segIdsSet ids = segments_->GetEnabledSegmentIDs();
    FOR_EACH(iter, ids){
        segs_.push_back( segments_->GetSegment( *iter ) );
    }
}

void segmentIterator::iterOverSegmentIDs(const common::segIdsSet & set)
{
    FOR_EACH(iter, set){
        segs_.push_back( segments_->GetSegment( *iter ) );
    }
}

bool segmentIterator::empty(){
    return segs_.empty();
}

segment* segmentIterator::getNextSegment()
{
    if( segs_.empty() ){
        return NULL;
    }

    segment* segRet = segs_.back();
    segs_.pop_back();

    const segChildCont_t& children = segRet->GetChildren();

    FOR_EACH(iter, children)
    {
        segs_.push_back(*iter);
    }

    return segRet;
}

segmentIterator & segmentIterator::operator = (const segmentIterator & other)
{
    if (this == &other) {
        return *this;
    }

    segments_ = other.segments_;
    segs_ = other.segs_;

    return *this;
}
