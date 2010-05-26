#ifndef OM_SEGMENT_ITERATOR_H
#define OM_SEGMENT_ITERATOR_H

#include "common/omCommon.h"
#include "segment/omSegmentCache.h"

class OmSegment;
class OmSegmentCache;

class OmSegmentIterator
{
 public:
	OmSegmentIterator( OmSegmentCache * cache );
	void iterOverSelectedIDs();
	void iterOverEnabledIDs();

	OmSegment * getNextSegment();
	bool empty();
	
	OmSegmentIterator & operator = (const OmSegmentIterator & other);

 private:
	OmSegmentCache * mCache;

	OmSegPtrs mSegs;
};

#endif
