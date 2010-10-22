#ifndef OM_SEGMENT_LISTS_HPP
#define OM_SEGMENT_LISTS_HPP

#include "segment/lowLevel/omSegmentListBySize.h"
#include "segment/lowLevel/omSegmentListByMRU.h"
#include "segment/details/omSegmentListContainer.hpp"

class OmSegmentLists {
private:
	OmSegmentListContainer<OmSegmentListBySize> validList_;
	OmSegmentListContainer<OmSegmentListBySize> workingList_;
	OmSegmentListContainer<OmSegmentListBySize> uncertainList_;
	OmSegmentListByMRU recentList_;

	OmSegmentListContainer<OmSegmentListBySize>&
	getContainer(const om::OmSegListType type)
	{
		switch(type){
		case om::VALID:
			return validList_;
		case om::WORKING:
			return workingList_;
		case om::UNCERTAIN:
			return uncertainList_;
		default:
			throw OmArgException("unknown type");
		}
	}

public:
	OmSegmentListByMRU& Recent() {
		return recentList_;
	}

	OmSegmentListContainer<OmSegmentListBySize>& Working() {
		return workingList_;
	}

	OmSegmentListContainer<OmSegmentListBySize>& Valid() {
		return validList_;
	}

	OmSegmentListContainer<OmSegmentListBySize>& Uncertain() {
		return uncertainList_;
	}

	void MoveSegment(const om::OmSegListType toType,
					 OmSegment* seg)
	{
		getContainer(seg->GetListType()).swapSegment(seg, getContainer(toType));
		seg->SetListType(toType);
	}

	void TouchRecentList(const OmSegID segID){
		recentList_.touch( segID );
	}

	void InsertSegmentWorking(OmSegment* seg){
		workingList_.insertSegment(seg);
	}

	void InsertSegmentValid(OmSegment* seg){
		validList_.insertSegment(seg);
	}

	void InsertSegmentUncertain(OmSegment* seg){
		uncertainList_.insertSegment(seg);
	}

	uint64_t GetNumTopLevelSegs() {
		return workingList_.size() + validList_.size() + uncertainList_.size();
	}

	void UpdateFromJoinWorking(OmSegment* root, OmSegment* child){
		workingList_.updateFromJoin(root, child);
	}

	void UpdateFromJoinValid(OmSegment* root, OmSegment* child){
		validList_.updateFromJoin(root, child);
	}

	void UpdateFromSplitWorking(OmSegment* root, OmSegment* child,
								const uint64_t newChildSize){
		workingList_.updateFromSplit(root, child, newChildSize);
	}
};

#endif
