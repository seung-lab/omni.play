#include "segment/omSegmentEdge.h"
#include "segment/omSegment.h"

OmSegmentEdge::OmSegmentEdge()
	: valid(false)
{}
	
OmSegmentEdge::OmSegmentEdge( OmSegment * c )
	: parentID(c->getParentSegID())
	, childID(c->getValue())
	, threshold(c->getThreshold())
	, valid(true)
{
}

OmSegmentEdge::OmSegmentEdge( const OmSegID p, const OmSegID c, const float t )
	: parentID(p)
	, childID(c)
	, threshold(t)
	, valid(true)
{
}

OmSegmentEdge::OmSegmentEdge( OmSegment * p, OmSegment * c, const float t )
	: parentID(p->getValue())
	, childID(c->getValue())
	, threshold(t)
	, valid(true)
{
}
	
bool OmSegmentEdge::isValid()
{ 
	return valid; 
}

bool OmSegmentEdge::operator==(const OmSegmentEdge & rhs) const
{
	return (parentID == rhs.parentID && 
		childID == rhs.childID &&
		threshold == rhs.threshold &&
		valid == rhs.valid );
}
