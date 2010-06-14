#include "segment/helpers/omSegmentListBySize.h"

void OmSegmentListBySize::insertSegment( OmSegment * seg )
{
	do_insertSegment( seg->mValue, seg->mSize );
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

OmSegIDsListWithPage * 
OmSegmentListBySize::getAPageWorthOfSegmentIDs( const unsigned int offset, const int numToGet, const OmSegID startSeg)
{
	OmSegIDsList ret = OmSegIDsList();

	List_by_size & sizeIndex = mList.get<segSize>();
	List_by_size::iterator iterSize = sizeIndex.begin();

	int counter = 0;
	int page = 0;
	if(0 == startSeg) {
		advance(iterSize, offset);
	 	page = offset / numToGet;
	} else {
        	for(; iterSize != sizeIndex.end(); ++iterSize) {
                	if(iterSize->segID == startSeg) {
                        	break;
                	}
			++counter;
		}
	 	advance(iterSize, -(counter % numToGet));
	 	page = counter / numToGet;
        }
	
	for( int i = 0; i < numToGet && iterSize != sizeIndex.end(); ++i, ++iterSize ){
		ret.push_back( iterSize->segID );
	}
	
	return new OmSegIDsListWithPage(ret, page);
}
