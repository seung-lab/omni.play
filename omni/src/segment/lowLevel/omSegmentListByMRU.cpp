#include "segment/lowLevel/omSegmentListByMRU.h"

OmSegmentListByMRU::OmSegmentListByMRU()
	: OmSegmentListBySize()
{
}

void OmSegmentListByMRU::touch( const OmSegID segID_ )
{
	const quint64 activity = getRecentActivity();

	List_by_ID & idIndex = mList.get<segID>();
	List_by_ID::iterator iter = idIndex.find(segID_);
	if(iter != idIndex.end() ){
		idIndex.replace( iter, OmSegSize(segID_, activity) );
	} else {
		mList.insert( OmSegSize(segID_, activity ) );
	}
}

OmSegIDsListWithPage * OmSegmentListByMRU::getAPageWorthOfSegmentIDs( const unsigned int offset, const int numToGet, const OmSegID startSeg)
{
	return OmSegmentListBySize::getAPageWorthOfSegmentIDs(offset, numToGet, startSeg);
}

quint64 OmSegmentListByMRU::getRecentActivity()
{
	static quint64 activity = 0;
	++activity;
	return activity;
}
