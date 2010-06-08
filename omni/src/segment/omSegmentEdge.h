#ifndef OM_SEGMENT_EDGE_H
#define OM_SEGMENT_EDGE_H

#include "common/omCommon.h"
#include "segment/omSegment.h"

class OmSegmentEdge {
 public:
	OmSegmentEdge(){}

 	OmSegmentEdge( const OmSegID p, const OmSegID c, const float t )
		: parentID(p)
		, childID(c)
		, threshold(t)
	{
	}

 	OmSegmentEdge( OmSegment * p, OmSegment * c, const float t )
		: parentID(p->getValue())
		, childID(c->getValue())
		, threshold(t)
	{
	}
	
	OmSegID parentID;
	OmSegID childID;
	float threshold;
};

#endif
