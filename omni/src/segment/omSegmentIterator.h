#ifndef OM_SEGMENT_ITERATOR_H
#define OM_SEGMENT_ITERATOR_H

#include "common/omCommon.h"
#include "segment/omSegmentPointers.h"
#include <deque>

class OmSegment;
class OmSegmentCache;

class OmSegmentIterator
{
 public:
	OmSegmentIterator( OmSegmentCache *, 
			   const bool iterOverSelectedIDs = false, 
			   const bool iterOverEnabledIDs = false );
 
	void iterOverSelectedIDs();
	void iterOverEnabledIDs();
	void iterOverSegmentIDs(const OmSegIDsSet & set);


	OmSegment * getNextSegment();
	bool empty();
	
	OmSegmentIterator & operator = (const OmSegmentIterator & other);

 private:
	OmSegmentCache * mCache;

	std::deque<OmSegment*> mSegs;
};

#endif
