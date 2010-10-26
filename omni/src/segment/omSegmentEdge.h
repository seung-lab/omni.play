#ifndef OM_SEGMENT_EDGE_H
#define OM_SEGMENT_EDGE_H

#include "common/omCommon.h"

class OmSegment;

class OmSegmentEdge {
 public:
	OmSegmentEdge();
	explicit OmSegmentEdge( OmSegment * c );
	OmSegmentEdge( const OmSegID p, const OmSegID c, const double t );
 	OmSegmentEdge( OmSegment * p, OmSegment * c, const double t );

	bool operator==( const OmSegmentEdge& rhs ) const;

	bool isValid() const {return valid;}

	OmSegID parentID;
	OmSegID childID;
	double threshold;
	bool valid;

	friend class QDataStream &operator<<(QDataStream & out, const OmSegmentEdge & e );
	friend class QDataStream &operator>>(QDataStream & in,  OmSegmentEdge & e );

};

#endif
