#ifndef OM_SEGMENT_POINTERS_H
#define OM_SEGMENT_POINTERS_H

#include "segment/omSegment.h"

typedef std::vector<OmSegment*> OmSegPtrList;

class OmSegPtrListValid {
 public:
 	OmSegPtrListValid()
		: isValid(false) 
		, freshness(0)
		{}
 	OmSegPtrListValid( const OmSegPtrList & L, const quint32 f )
		: isValid(true)
		, list(L) 
		, freshness(f)
	{}
	
	bool isValid;
	OmSegPtrList list;
	quint32 freshness;
};

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
