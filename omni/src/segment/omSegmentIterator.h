#ifndef OM_SEGMENT_ITERATOR_H
#define OM_SEGMENT_ITERATOR_H

#include "common/omCommon.h"
#include "segment/omSegmentPointers.h"
#include <deque>

class OmSegment;
class OmSegmentCache;

class OmSegmentIterator {
public:
	explicit OmSegmentIterator(OmSegmentCache*);

	void iterOverSegmentID(const OmSegID segID);
	void iterOverSegmentIDs(const OmSegIDsSet& set);
	void iterOverSelectedIDs();
	void iterOverEnabledIDs();

	OmSegment* getNextSegment();
	bool empty();

	OmSegmentIterator & operator = (const OmSegmentIterator & other);

private:
	OmSegmentCache* mCache;
	std::deque<OmSegment*> mSegs;
};

#endif
