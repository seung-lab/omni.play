#ifndef SORTED_ROOT_SEGMENTS_VECTOR_HPP
#define SORTED_ROOT_SEGMENTS_VECTOR_HPP

#include "common/omCommon.h"
#include "segment/details/sortedRootSegmentsTypes.h"
#include "zi/omUtility.h"

class SortedRootSegmentsVector{
private:
	typedef om::sortedRootSegments_::Node Node;
	std::vector<Node> sortedVec_;

	typedef om::sortedRootSegments_::bm_type bm_type;

public:
	SortedRootSegmentsVector()
	{}

	void Clear(){
		sortedVec_.clear();
	}

	void Dump() const
	{
		printf("vector:\n");
		for( size_t i = 0; i < sortedVec_.size(); ++i){
			std::cout << "\t" << sortedVec_[i] << "\n";
		}
	}

	void RegenSortedVector(const bm_type& sortedMap)
	{
		sortedVec_.resize(sortedMap.size());
		int i = 0;
		FOR_EACH(iter, sortedMap.left){
			sortedVec_[i++] = iter->first;
		}
	}

	boost::shared_ptr<OmSegIDsListWithPage>
	GetSimplePageOfSegmentIDs(const uint32_t offset, const int numToGet)
	{
		boost::shared_ptr<OmSegIDsList> ret =
			boost::make_shared<OmSegIDsList>();

		for(int i = 0; i < numToGet && (i+offset) < sortedVec_.size(); ++i){
			ret->push_back(sortedVec_[i+offset].segID);
		}

		return boost::make_shared<OmSegIDsListWithPage>(ret, offset/numToGet);
	}

	boost::shared_ptr<OmSegIDsListWithPage>
	GetPageContainingSegment(const OmSegID startSegID,
							 const uint64_t startSegSize,
							 const uint32_t numToGet)
	{
		const Node n = { startSegID, startSegSize };

		std::vector<Node>::iterator iter
			= std::lower_bound(sortedVec_.begin(),
							   sortedVec_.end(),
							   n,
							   om::sortedRootSegments_::cmpNode);

		if(iter == sortedVec_.end()){
			throw OmIoException("could not find segment");
		}

		assert(*iter == n);

		const uint32_t offset = iter - sortedVec_.begin();
		const uint32_t roundedOffset = ROUNDDOWN(offset, numToGet);

		return GetSimplePageOfSegmentIDs(roundedOffset, numToGet);
	}
};

#endif
