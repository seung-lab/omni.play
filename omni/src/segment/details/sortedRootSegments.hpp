#ifndef SORTED_ROOT_SEGMENTS_HPP
#define SORTED_ROOT_SEGMENTS_HPP

#include "common/omCommon.h"
#include "segment/details/sortedRootSegmentsVector.hpp"
#include "segment/details/sortedRootSegmentsTypes.h"

class SortedRootSegments{
private:
	typedef om::sortedRootSegments_::bm_type bm_type;
	bm_type sortedMap_;

	typedef om::sortedRootSegments_::Node Node;

	SortedRootSegmentsVector sortedVec_;
	bool sortedVecIsFresh_;

	inline void regenSortedVectorIfNeeded()
	{
		if(sortedVecIsFresh_){
			return;
		}

		sortedVec_.RegenSortedVector(sortedMap_);
		sortedVecIsFresh_ = true;
	}

public:
	SortedRootSegments()
		: sortedVecIsFresh_(false)
	{}

	void Add(const OmSegID segID, const uint64_t size)
	{
		sortedVecIsFresh_ = false;
		const Node n = { segID, size };
		sortedMap_.left.insert(bm_type::left_value_type(n, segID));
	}

	uint64_t Remove(const OmSegID segID)
	{
		sortedVecIsFresh_ = false;

		bm_type::right_iterator right_iter = sortedMap_.right.find(segID);
		if(right_iter == sortedMap_.right.end()){
			return 0;
		}
		const uint64_t size = right_iter->second.size;
		sortedMap_.right.erase(right_iter);
		return size;
	}

	uint64_t GetSegmentSize(const OmSegID segID) const
	{
		bm_type::right_const_iterator right_iter = sortedMap_.right.find(segID);
		if(right_iter == sortedMap_.right.end()){
			return 0;
		}
		return right_iter->second.size;
	}

	void Clear()
	{
		sortedVecIsFresh_ = false;
		sortedMap_.clear();
		sortedVec_.Clear();
	}

	void Dump() const
	{
		printf("map:\n");
		FOR_EACH(iter, sortedMap_.left){
			std::cout << "\t" << iter->first << ", " << iter->second << "\n";
		}

		sortedVec_.Dump();
	}

	size_t Size() const {
		return sortedMap_.size();
	}

	boost::shared_ptr<OmSegIDsListWithPage>
	GetSimplePageOfSegmentIDs(const uint32_t offset, const int numToGet)
	{
		regenSortedVectorIfNeeded();
		return sortedVec_.GetSimplePageOfSegmentIDs(offset, numToGet);
	}

	boost::shared_ptr<OmSegIDsListWithPage>
	GetPageContainingSegment(const OmSegID startSegID, const uint32_t numToGet)
	{
		regenSortedVectorIfNeeded();
		return sortedVec_.GetPageContainingSegment(startSegID,
												   GetSegmentSize(startSegID),
												   numToGet);
	}
};

#endif
