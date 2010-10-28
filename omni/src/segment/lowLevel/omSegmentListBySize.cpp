#include "segment/lowLevel/omSegmentListBySize.h"

void OmSegmentListBySize::insertSegment(OmSegment* seg)
{
	do_insertSegment(seg->value(), seg->getSize());
}

void OmSegmentListBySize::swapSegment(OmSegment* seg, OmSegmentListBySize& one,
									  OmSegmentListBySize& two)
{
	List_by_ID& idIndex = one.mList.get<segID>();
	List_by_ID::iterator iter = idIndex.find(seg->value());
	if(iter != idIndex.end()){
		two.do_insertSegment(seg->value(), iter->segSize);
		idIndex.erase(iter);
	}
}

void OmSegmentListBySize::removeSegment(OmSegment* seg)
{
	do_removeSegment(seg->value());
}

void OmSegmentListBySize::updateFromJoin(OmSegment* root, OmSegment* child)
{
	do_incrementSegSize(root->value(), getSegmentSize(child));
	do_removeSegment(child->value());
}

void OmSegmentListBySize::updateFromSplit(OmSegment* root, OmSegment* child,
										  const uint64_t newChildSize)
{
	do_incrementSegSize(root->value(), -newChildSize);
	do_insertSegment(child->value(), newChildSize);
}

void OmSegmentListBySize::do_incrementSegSize(const OmSegID segID_,
											  const uint64_t addedSize)
{
	List_by_ID& idIndex = mList.get<segID>();
	List_by_ID::iterator iter = idIndex.find(segID_);
	if(iter != idIndex.end()){
		const uint64_t newSize = iter->segSize + addedSize;
		idIndex.erase(iter);
		do_insertSegment(segID_, newSize);
	}
}

void OmSegmentListBySize::do_removeSegment(const OmSegID segID_)
{
	List_by_ID& idIndex = mList.get<segID>();
	List_by_ID::iterator iter = idIndex.find(segID_);
	if(iter != idIndex.end()){
		idIndex.erase(iter);
	}
}

void OmSegmentListBySize::do_insertSegment(const OmSegID segID_,
										   const uint64_t size_)
{
	mList.insert(OmSegSize(segID_, size_));
}

void OmSegmentListBySize::advanceIter(List_by_size& sizeIndex,
									  List_by_size::iterator& iterSize,
									  const int offset)
{
	int direction = 1;
	if(offset < 0) {
		direction = -1;
	}

	if(1 == direction) {
		for(int i=0; i < offset*direction && iterSize != sizeIndex.end(); ++i)
		{
		 	++iterSize;
		}
	} else {
		for(int i=0; i < offset*direction && iterSize != sizeIndex.begin(); ++i)
		{
		 	--iterSize;
		}
	}
}

boost::shared_ptr<OmSegIDsListWithPage>
OmSegmentListBySize::getPageOfSegmentIDs(const unsigned int offset,
										 const int numToGet,
										 const OmSegID startSeg)
{
	List_by_size& sizeIndex = mList.get<segSize>();
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

	boost::shared_ptr<OmSegIDsList> ret = boost::make_shared<OmSegIDsList>();

	for(int i=0; i < numToGet && iterSize != sizeIndex.end(); ++i, ++iterSize){
		ret->push_back(iterSize->segID);
	}

	return boost::make_shared<OmSegIDsListWithPage>(ret, page);
}

uint64_t OmSegmentListBySize::getSegmentSize(OmSegment* seg)
{
	List_by_ID& idIndex = mList.get<segID>();
	List_by_ID::iterator iter = idIndex.find(seg->value());
	if(iter != idIndex.end()){
		return iter->segSize;
	}

	return seg->getSize();
}

void OmSegmentListBySize::dump()
{
	List_by_size& sizeIndex = mList.get<segSize>();
	List_by_size::iterator iterSize = sizeIndex.begin();

	for(; iterSize != sizeIndex.end(); ++iterSize){
		printf("seg %d, size %ld\n", iterSize->segID, iterSize->segSize);
	}
}

size_t OmSegmentListBySize::size()
{
	return mList.size();
}

void OmSegmentListBySize::clear()
{
	mList.clear();
}

OmSegID OmSegmentListBySize::GetNextSegmentIDinList(const OmSegID id)
{
	List_by_ID& idIndex = mList.get<segID>();
	List_by_ID::iterator iter = idIndex.find(id);

	if(iter == idIndex.end()){
		return 0;
	}

	++iter;

	if(iter == idIndex.end()){
		return 0;
	}

	return iter->segID;
}
