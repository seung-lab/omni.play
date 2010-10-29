#ifndef OM_SEGMENT_LIST_BY_SIZE2_H
#define OM_SEGMENT_LIST_BY_SIZE2_H

#include "common/omCommon.h"
#include "segment/omSegment.h"
#include "segment/omSegmentEdge.h"
#include "segment/details/omSegmentListsTypes.hpp"

class OmSegmentListBySize2 {
public:
	OmSegmentListBySize2(const int size)
	{
		sizes_.resize(size);
	}

	void InsertSegment(OmSegment* seg)
	{
		InsertSegment(seg->value(), seg->size());
	}

	void InsertSegment(const OmSegID segID, const uint64_t size)
	{
		if(segID >= sizes_.size()){
			sizes_.resize(2 * segID);
		}
		sizes_[segID] = size;
	}

	void RemoveSegment(OmSegment* seg)
	{
		sizes_[seg->value()] = 0;
	}

	void UpdateFromJoin(OmSegment* root, OmSegment* child)
	{
		sizes_[root->value()] += child->size();
		sizes_[child->value()] = 0;
	}

	void UpdateFromSplit(OmSegment* root, OmSegment* child,
						 const uint64_t newChildSize)
	{
		sizes_[root->value()] -= newChildSize;
		sizes_[child->value()] += newChildSize; // or just = ??
	}

	static void SwapSegment(OmSegment* seg, OmSegmentListBySize2 & one,
							OmSegmentListBySize2 & two)
	{
		const OmSegID segID = seg->value();
		const uint64_t size = one.GetSegmentSize(seg);

		two.InsertSegment(segID, size);
		one.RemoveSegment(seg);
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
		return sizes_[seg->value()];
	}

	size_t Size()
	{
		return sizes_.size();
	}

	void Dump()
	{
		for( size_t i = 0; i < sizes_.size(); ++i){
			std::cout << "seg " << i << ", size " << sizes_[i] << "\n";
		}
	}

	void Clear()
	{
		sizes_.clear();
	}

	OmSegID GetNextSegmentIDinList(const OmSegID id)
	{
		return 0;
	}

protected:

	std::vector<uint64_t> sizes_;
};

#endif
