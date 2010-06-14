#include "segment/helpers/omSegmentListByMRU.h"

OmSegmentListByMRU::OmSegmentListByMRU()
	: OmSegmentListBySize()
{
}

void OmSegmentListByMRU::touch( const OmSegID segID_, const quint64 activity_ )
{
	List_by_ID & idIndex = mList.get<segID>();
	List_by_ID::iterator iter = idIndex.find(segID_);
	if(iter != idIndex.end() ){
		const quint64 newSize = iter->segSize + activity_;
		idIndex.replace( iter, OmSegSize(segID_, newSize) );
	} else {
		mList.insert( OmSegSize(segID_, activity_ ) );
	}
}

OmSegIDsListWithPage * OmSegmentListByMRU::getAPageWorthOfSegmentIDs( const unsigned int offset, const int numToGet, const OmSegID startSeg)
{
	return OmSegmentListBySize::getAPageWorthOfSegmentIDs(offset, numToGet, startSeg);
}
