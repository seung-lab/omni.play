#ifndef OM_SEGMENT_LISTS_HPP
#define OM_SEGMENT_LISTS_HPP

#include "segment/lowLevel/omSegmentListBySize.h"
#include "segment/lowLevel/omSegmentListByMRU.h"

class OmSegment;
class OmSegmentation;

template <typename T>
class OmSegmentListContainer {
public:
	void insertSegment(OmSegment* seg){
		list.insertSegment(seg);
	}
	void clear(){
		list.clear();
	}
	size_t size(){
		return list.size();
	}
	void updateFromJoin(OmSegment* root, OmSegment* child){
		list.updateFromJoin(root, child);
	}
	OmSegIDsListWithPage* getAPageWorthOfSegmentIDs(const uint32_t offset,
							const int numToGet,
							const OmSegID startSeg){
		return list.getAPageWorthOfSegmentIDs(offset, numToGet, startSeg);
	}
	void updateFromSplit(OmSegment* root, OmSegment* child, const quint64 newChildSize){
		list.updateFromSplit(root, child, newChildSize);
	}
	void swapSegment(OmSegment* seg, OmSegmentListContainer<T>& two ){
		T::swapSegment(seg, list, two.list);
	}
	quint64 getSegmentSize(OmSegment* seg){
		return list.getSegmentSize(seg);
	}

private:
	T list;
};

class OmSegmentLists {
public:
	OmSegmentLists(){}

	OmSegmentListContainer<OmSegmentListBySize> mValidListBySize;
	OmSegmentListContainer<OmSegmentListBySize> mRootListBySize;
	OmSegmentListByMRU mRecentRootActivityMap;

	void moveSegmentFromValidToRoot(OmSegment* seg){
		mValidListBySize.swapSegment(seg, mRootListBySize);
	}

	void moveSegmentFromRootToValid(OmSegment* seg){
		mRootListBySize.swapSegment(seg, mValidListBySize);
	}
};

#endif
