#ifndef OM_SEGMENT_ITERATOR_H
#define OM_SEGMENT_ITERATOR_H

#include "common/omCommon.h"
#include "segment/omSegmentPointers.h"

class OmSegment;
class OmSegments;

class OmSegmentIterator {
public:
    explicit OmSegmentIterator(OmSegments*);

    void iterOverSegmentID(const OmSegID segID);
    void iterOverSegmentIDs(const OmSegIDsSet& set);
    void iterOverSelectedIDs();
    void iterOverEnabledIDs();

    OmSegment* getNextSegment();
    bool empty();

    OmSegmentIterator & operator = (const OmSegmentIterator & other);

private:
    OmSegments* mCache;
    std::deque<OmSegment*> segs_;
};

#endif
