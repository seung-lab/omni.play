#ifndef OM_SEGMENT_QUEUE_H
#define OM_SEGMENT_QUEUE_H

#include "common/omCommon.h"
#include <queue>
#include <map>

typedef struct {
	OmSegID segID;
	float threshold;
} OmSegmentQueueElement;

class OmSegmentQueue {
 public:
	OmSegmentQueueElement top();
	void pop();
	void push( const OmSegmentQueueElement & e);
	bool empty();
	void remove( const OmSegID & segID, const float & threshold );

 private:
	std::multimap<float, OmSegID, std::greater<float> > map;

};

//std::priority_queue< OmSegQueueElement, std::vector<OmSegQueueElement>, OmSegQueueComparator >


#endif
