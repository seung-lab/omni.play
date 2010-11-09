#ifndef OM_SEGMENT_LIST_BY_SIZE2_H
#define OM_SEGMENT_LIST_BY_SIZE2_H

#include "common/omCommon.h"
#include "segment/omSegment.h"
#include "segment/omSegmentEdge.h"
#include "segment/details/omSegmentListsTypes.hpp"
#include "segment/details/sortedRootSegments.hpp"

class OmSegmentListBySize2 {
private:
	SortedRootSegments sortedRoots_;

public:
	OmSegmentListBySize2()
	{}

	void InsertSegment(OmSegment* seg){
		InsertSegment(seg->value(), seg->size());
	}

	void InsertSegment(const OmSegID segID, const uint64_t size){
		sortedRoots_.Add(segID, size);
	}

	void UpdateFromJoin(OmSegment* root, OmSegment* child)
	{
		const OmSegID childID = child->value();
		const uint64_t childSize = sortedRoots_.Remove(childID);

		const OmSegID rootID = root->value();
		const uint64_t oldRootSize = sortedRoots_.Remove(rootID);
		sortedRoots_.Add(rootID, oldRootSize + childSize);
	}

	void UpdateFromSplit(OmSegment* root, OmSegment* child,
						 const uint64_t newChildSize)
	{
		const OmSegID childID = child->value();
		sortedRoots_.Add(childID, newChildSize);

		const OmSegID rootID = root->value();
		const uint64_t oldRootSize = sortedRoots_.Remove(rootID);
		sortedRoots_.Add(rootID, oldRootSize - newChildSize);
	}

	static void SwapSegment(OmSegment* seg, OmSegmentListBySize2& one,
							OmSegmentListBySize2& two)
	{
		const OmSegID segID = seg->value();
		const uint64_t segSize = one.sortedRoots_.Remove(segID);
		two.InsertSegment(segID, segSize);
	}

	boost::shared_ptr<OmSegIDsListWithPage>
	GetPageOfSegmentIDs(const uint32_t offset,
						const int numToGet,
						const OmSegID startSeg )
	{
		if(0 == startSeg) {
			return sortedRoots_.GetSimplePageOfSegmentIDs(offset, numToGet);
		}

		return sortedRoots_.GetPageContainingSegment(startSeg, numToGet);
	}

	uint64_t GetSegmentSize(OmSegment* seg){
		return sortedRoots_.GetSegmentSize(seg->value());
	}

	size_t Size(){
		return sortedRoots_.Size();
	}

	void Dump(){
		sortedRoots_.Dump();
	}

	void Clear(){
		sortedRoots_.Clear();
	}

	OmSegID GetNextSegmentIDinList(const OmSegID /*id*/)
	{
		assert(0 && "fill me in");
		return 0;
	}
};

#endif
