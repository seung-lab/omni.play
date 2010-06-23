#ifndef OM_SEGMENT_ITERATOR_LOW_LEVEL_H
#define OM_SEGMENT_ITERATOR_LOW_LEVEL_H

#include "common/omCommon.h"
#include "segment/omSegmentPointers.h"

class OmSegment;
class OmSegmentCacheImpl;

class OmSegmentIteratorLowLevel {

 public:
	OmSegmentIteratorLowLevel( OmSegmentCacheImpl * );
	void iterOverSegmentID(const OmSegID segID);

	OmSegment * getNextSegment();
	bool empty();
	
	OmSegmentIteratorLowLevel & operator = (const OmSegmentIteratorLowLevel & other);

 private:
	OmSegmentCacheImpl * mCache;
	OmSegPtrList mSegs;
};

#endif
