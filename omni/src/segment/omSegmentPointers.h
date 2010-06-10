#ifndef OM_SEGMENT_POINTERS_H
#define OM_SEGMENT_POINTERS_H

#include "segment/omSegment.h"

typedef std::vector<OmSegment*> OmSegPtrList;

class OmSegPtrListValid {
 public:
	OmSegPtrListValid()
		: isValid(false) {}
	OmSegPtrListValid( const OmSegPtrList & L )
		: isValid(true), list(L) {}
	
	bool isValid;
	OmSegPtrList list;
};

class OmSegPtrListWithPage {
 public:
	OmSegPtrListWithPage( const OmSegPtrList & L, int pageOffset )
		: mPageOffset(pageOffset)
		, list(L) {}
	
	int mPageOffset;
	OmSegPtrList list;
};


#endif
