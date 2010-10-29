#ifndef OM_SEGMENT_LIST_BY_SIZE2_H
#define OM_SEGMENT_LIST_BY_SIZE2_H

#include "common/omCommon.h"
#include "segment/omSegment.h"
#include "segment/omSegmentEdge.h"
#include "segment/details/omSegmentListsTypes.hpp"
#include "segment/details/sortedRootSegments.hpp"

class OmSegmentListBySize2 {
public:
	OmSegmentListBySize2()
	{}

	OmSegmentListBySize2(const int size)
	{
		allSegmentSizes_.resize(size);
	}

	void InsertSegment(OmSegment* seg){
		InsertSegment(seg->value(), seg->size());
	}


	void InsertSegment(const OmSegID segID, const uint64_t size)
	{
		if(segID >= allSegmentSizes_.size()){
			allSegmentSizes_.resize(2 * segID);
		}
		allSegmentSizes_[segID] = size;

		sortedRoots_.Add(segID, size);
	}

	void UpdateFromJoin(OmSegment* root, OmSegment* child)
	{
		const OmSegID rootID = root->value();
		const OmSegID childID = child->value();
		const uint64_t childSize = allSegmentSizes_[childID];
		const uint64_t oldRootSize = allSegmentSizes_[rootID];

		allSegmentSizes_[rootID] += childSize;
		allSegmentSizes_[childID] = 0;

		sortedRoots_.Remove(rootID, oldRootSize);
		sortedRoots_.Remove(childID, childSize);

		sortedRoots_.Add(rootID, allSegmentSizes_[rootID]);
	}

	void UpdateFromSplit(OmSegment* root, OmSegment* child,
						 const uint64_t newChildSize)
	{
		const OmSegID rootID = root->value();
		const uint64_t oldRootSize = allSegmentSizes_[rootID];
		sortedRoots_.Remove(rootID, oldRootSize);

		const OmSegID childID = child->value();
		allSegmentSizes_[root->value()] -= newChildSize;
		allSegmentSizes_[childID] += newChildSize; // or just = ??

		sortedRoots_.Add(rootID, allSegmentSizes_[rootID]);
		sortedRoots_.Add(childID, newChildSize);
	}

	static void SwapSegment(OmSegment* seg, OmSegmentListBySize2 & one,
							OmSegmentListBySize2 & two)
	{
		const OmSegID segID = seg->value();
		const uint64_t size = one.GetSegmentSize(seg);

		two.InsertSegment(segID, size);
		one.removeSegment(seg);
	}

	boost::shared_ptr<OmSegIDsListWithPage>
	GetPageOfSegmentIDs(const unsigned int offset, const int numToGet,
						const OmSegID startSeg)
	{
		boost::shared_ptr<OmSegIDsList> ret =
			boost::make_shared<OmSegIDsList>();

		return boost::make_shared<OmSegIDsListWithPage>(ret, 0);
	}

	uint64_t GetSegmentSize(OmSegment* seg)
	{
		return allSegmentSizes_[seg->value()];
	}

	size_t Size()
	{
		return sortedRoots_.Size();
	}

	void Dump()
	{
		sortedRoots_.Dump();
	}

	void Clear()
	{
		allSegmentSizes_.clear();
		sortedRoots_.Clear();
	}

	OmSegID GetNextSegmentIDinList(const OmSegID id)
	{
		return 0;
	}

protected:

	std::vector<uint64_t> allSegmentSizes_;
	SortedRootSegments sortedRoots_;

private:
	void removeSegment(OmSegment* seg)
	{
		const OmSegID segID = seg->value();
		const uint64_t oldSize = allSegmentSizes_[segID];

		allSegmentSizes_[segID] = 0;
		sortedRoots_.Remove(segID, oldSize);
	}
};


#endif
