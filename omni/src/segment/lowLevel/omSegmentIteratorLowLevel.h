#ifndef OM_SEGMENT_ITERATOR_LOW_LEVEL_H
#define OM_SEGMENT_ITERATOR_LOW_LEVEL_H

#include "common/omCommon.h"
#include "segment/omSegmentPointers.h"

class OmSegment;
class OmSegmentCacheImplLowLevel;

class OmSegmentIteratorLowLevel : boost::noncopyable {

 public:
	OmSegmentIteratorLowLevel( OmSegmentCacheImplLowLevel * );

	void iterOverAllSegments();
	void iterOverSegmentID(const OmSegID segID);

	OmSegment * getNextSegment();
	bool empty();
	
 private:
	OmSegmentCacheImplLowLevel * mCache;
	OmSegPtrList mSegs;

	bool mIterOverAll;
	OmSegID mCurSegID;

	OmSegment * getNextSegmentFromFullList();
	OmSegment * getNextSegmentFromSet();
};

#endif
