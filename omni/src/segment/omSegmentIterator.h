#pragma once

#include "common/omCommon.h"
#include "segment/omSegmentPointers.h"

class OmSegment;
class OmSegments;
class SegmentDataWrapper;
class SegmentationDataWrapper;

class OmSegmentIterator {
public:
    explicit OmSegmentIterator(OmSegments*);
    explicit OmSegmentIterator(const SegmentationDataWrapper& sdw);
    explicit OmSegmentIterator(const SegmentDataWrapper& sdw);

    void iterOverSegmentID(const OmSegID segID);
    void iterOverSegmentIDs(const OmSegIDsSet& set);
    void iterOverSelectedIDs();
    void iterOverEnabledIDs();

    OmSegment* getNextSegment();
    bool empty();

    OmSegmentIterator & operator = (const OmSegmentIterator & other);

private:
    OmSegments* segments_;
    std::deque<OmSegment*> segs_;
};

