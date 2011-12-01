#pragma once

#include "common/common.h"
#include "segment/segmentPointers.h"

class segment;
class segments;
class SegmentDataWrapper;
class SegmentationDataWrapper;

class segmentIterator {
public:
    explicit segmentIterator(segments*);
    explicit segmentIterator(const SegmentationDataWrapper& sdw);
    explicit segmentIterator(const SegmentDataWrapper& sdw);

    void iterOverSegmentID(const common::segId segID);
    void iterOverSegmentIDs(const common::segIdSet& set);
    void iterOverSelectedIDs();
    void iterOverEnabledIDs();

    segment* getNextSegment();
    bool empty();

    segmentIterator & operator = (const segmentIterator & other);

private:
    segments* segments_;
    std::deque<segment*> segs_;
};

