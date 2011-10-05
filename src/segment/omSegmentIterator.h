#pragma once

#include "common/common.h"
#include "segment/omSegmentPointers.h"

class OmSegment;
class segments;
class SegmentDataWrapper;
class SegmentationDataWrapper;

class OmSegmentIterator {
public:
    explicit OmSegmentIterator(segments*);
    explicit OmSegmentIterator(const SegmentationDataWrapper& sdw);
    explicit OmSegmentIterator(const SegmentDataWrapper& sdw);

    void iterOverSegmentID(const segId segID);
    void iterOverSegmentIDs(const segIdsSet& set);
    void iterOverSelectedIDs();
    void iterOverEnabledIDs();

    OmSegment* getNextSegment();
    bool empty();

    OmSegmentIterator & operator = (const OmSegmentIterator & other);

private:
    segments* segments_;
    std::deque<OmSegment*> segs_;
};

