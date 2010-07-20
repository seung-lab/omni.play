#ifndef OM_SEGMENT_POINTERS_H
#define OM_SEGMENT_POINTERS_H

#include "segment/omSegment.h"

typedef std::vector<OmSegment*> OmSegPtrList;

class OmSegPtrListValid {
 public:
 	OmSegPtrListValid()
		: isValid(false) 
		, freshness(0)
		, isFetching(false)
	{}
 	explicit OmSegPtrListValid(const bool isFetching)
		: isValid(false) 
		, freshness(0)
		, isFetching(isFetching)
	{}
 	OmSegPtrListValid( const OmSegPtrList & L, const quint32 f )
		: isValid(true)
		, list(L) 
		, freshness(f)
		, isFetching(false)
	{}
	
	bool isValid;
	OmSegPtrList list;
	quint32 freshness;
	bool isFetching;
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
