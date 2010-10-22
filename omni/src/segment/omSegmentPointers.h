#ifndef OM_SEGMENT_POINTERS_H
#define OM_SEGMENT_POINTERS_H

#include "segment/omSegment.h"

typedef std::deque<OmSegment*> OmSegPtrList;

class OmSegIDsListWithPage {
 public:
	OmSegIDsListWithPage( const OmSegIDsList & L, int pageOffset )
		: mPageOffset(pageOffset)
		, list(L) {}

	int mPageOffset;
	OmSegIDsList list;
};


#endif
