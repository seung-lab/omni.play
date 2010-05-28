#ifndef OM_SEGMENT_POINTERS_H
#define OM_SEGMENT_POINTERS_H

#include "segment/omSegment.h"

typedef std::vector<OmSegment*> OmSegPtrs;

class OmSegPtrsValid {
 public:
	OmSegPtrsValid()
		: isValid(false) {}
	OmSegPtrsValid( const OmSegPtrs & L )
		: isValid(true), list(L) {}
	
	bool isValid;
	OmSegPtrs list;
};


#endif
