#ifndef OM_SEGMENT_EDGE_H
#define OM_SEGMENT_EDGE_H

#include "common/omCommon.h"

class OmSegmentEdge {
 public:
 	OmSegmentEdge( const OmSegID p, const OmSegID c, const float t )
		: parentID(p)
		, childID(c)
		, threshold(t)
		, hasBeenJoined(false)
	{
	}
	
	OmSegID parentID;
	OmSegID childID;
	float threshold;

	bool hasBeenJoined;
};

#endif
