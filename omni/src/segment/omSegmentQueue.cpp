#include "segment/omSegmentQueue.h"

OmSegmentQueueElement OmSegmentQueue::top()
{
	assert(!map.empty());

	std::multimap<float, OmSegID >::const_iterator iter = map.begin();

	OmSegmentQueueElement ret = { (*iter).second,
				      (*iter).first };

	return ret;
}

void OmSegmentQueue::pop()
{
	assert(!map.empty());
	std::multimap<float, OmSegID >::iterator iter = map.begin();
	map.erase(iter);
}

void OmSegmentQueue::push( const OmSegmentQueueElement & e)
{
	map.insert( std::pair<float, OmSegID >( e.threshold, e.segID ));
}

int OmSegmentQueue::remove( const OmSegID & segID, const float & threshold )
{
	assert(!map.empty());

	int ret = 0;

	std::multimap<float, OmSegID >::iterator iter;
	for( iter = map.find(threshold); iter != map.end(); ++iter){
		if( segID == (*iter).second ){
			map.erase(iter);
			++ret;
		}
	}

	return ret;
}

