#include "segment/omSegmentEdge.h"
#include "segment/omSegment.h"

OmSegmentEdge::OmSegmentEdge()
  : parentID(0)
  , childID(0)
  , threshold(-100)
  , valid(false)
{}

OmSegmentEdge::OmSegmentEdge( OmSegment * c )
	: parentID(c->getParentSegID())
	, childID(c->value())
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
	: parentID(p->value())
	, childID(c->value())
	, threshold(t)
	, valid(true)
{
}

bool OmSegmentEdge::operator==(const OmSegmentEdge & rhs) const
{
	return (parentID == rhs.parentID &&
		childID == rhs.childID &&
		threshold == rhs.threshold &&
		valid == rhs.valid );
}
