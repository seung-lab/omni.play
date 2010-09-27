#ifndef OM_SEGMENT_POINTERS_H
#define OM_SEGMENT_POINTERS_H

#include "segment/omSegment.h"
#include <deque>

typedef std::deque<OmSegment*> OmSegPtrList;
typedef boost::shared_ptr<OmSegPtrList> OmSegPtrListPtr;

class OmSegPtrListWithPage {
 public:
	OmSegPtrListWithPage( const OmSegPtrList & L, int pageOffset )
		: mPageOffset(pageOffset)
		, list(L) {}

	int mPageOffset;
	OmSegPtrList list;
};

class OmSegIDsListWithPage {
 public:
	OmSegIDsListWithPage( const OmSegIDsList & L, int pageOffset )
		: mPageOffset(pageOffset)
		, list(L) {}

	int mPageOffset;
	OmSegIDsList list;
};


#endif
