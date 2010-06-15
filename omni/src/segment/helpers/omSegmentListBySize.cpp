#include "segment/helpers/omSegmentListBySize.h"

void OmSegmentListBySize::insertSegment( OmSegment * seg )
{
	do_insertSegment( seg->mValue, seg->mSize );
}

void OmSegmentListBySize::removeSegment( OmSegment * seg )
{
        do_removeSegment( seg->mValue );
}

void OmSegmentListBySize::updateFromJoin( OmSegment * root, OmSegment * child )
{
	do_incrementSegSize( root->mValue, child->mSize );
	do_removeSegment( child->mValue );
}
	
void OmSegmentListBySize::do_incrementSegSize( const OmSegID segID_, const quint64 addedSize )
{
	List_by_ID & idIndex = mList.get<segID>();
	List_by_ID::iterator iter = idIndex.find(segID_);
	if(iter != idIndex.end() ){
		const quint64 newSize = iter->segSize + addedSize;
		idIndex.replace( iter, OmSegSize(segID_, newSize) );
	}
}

void OmSegmentListBySize::do_removeSegment( const OmSegID segID_ )
{
	List_by_ID & idIndex = mList.get<segID>();
	List_by_ID::iterator iter = idIndex.find(segID_);
	if(iter != idIndex.end() ){
		idIndex.erase(iter);
	}
}

void OmSegmentListBySize::do_insertSegment( const OmSegID segID_, const quint64 size_ )
{
	mList.insert( OmSegSize(segID_, size_ ) );
}

void OmSegmentListBySize::advanceIter(List_by_size & sizeIndex, List_by_size::iterator & iterSize, const int offset)
{
	int direction = 1;
	if(offset < 0) {
		direction = -1;
	}
	
	if(1 == direction) {
		for(int i = 0; i < offset*direction && iterSize != sizeIndex.end(); i++)
		{
		 	++iterSize;
		}
	} else {
		for(int i = 0; i < offset*direction && iterSize != sizeIndex.begin(); i++)
		{
		 	--iterSize;
		}
	}
}

OmSegIDsListWithPage * 
OmSegmentListBySize::getAPageWorthOfSegmentIDs( const unsigned int offset, const int numToGet, const OmSegID startSeg)
{
	OmSegIDsList ret = OmSegIDsList();

	List_by_size & sizeIndex = mList.get<segSize>();
	List_by_size::iterator iterSize = sizeIndex.begin();

	int counter = 0;
	int page = 0;
	if(0 == startSeg) {
		advanceIter(sizeIndex, iterSize, offset);
	 	page = offset / numToGet;
	} else {
        	for(; iterSize != sizeIndex.end(); ++iterSize) {
                	if(iterSize->segID == startSeg) {
                        	break;
                	}
			++counter;
		}
	 	advanceIter(sizeIndex, iterSize, -(counter % numToGet));
	 	page = counter / numToGet;
        }
	
	for( int i = 0; i < numToGet && iterSize != sizeIndex.end(); ++i, ++iterSize ){
		ret.push_back( iterSize->segID );
	}
	
	return new OmSegIDsListWithPage(ret, page);
}
