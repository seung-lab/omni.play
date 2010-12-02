#ifndef OM_SEGMENT_LIST_BY_SIZE_H
#define OM_SEGMENT_LIST_BY_SIZE_H

#include "common/omCommon.h"
#include "segment/omSegment.h"
#include "segment/omSegmentEdge.h"
#include "segment/details/omSegmentListsTypes.hpp"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>

class OmSegmentListBySize {
public:
	OmSegmentListBySize()
		: mVoxels(0)
	{}

	void InsertSegment(OmSegment* seg);
	void RemoveSegment(OmSegment* seg);
	void UpdateFromJoin(OmSegment* root, OmSegment* child);
	void UpdateFromSplit(OmSegment* root, OmSegment* child, const int64_t);

	static void SwapSegment(OmSegment* seg, OmSegmentListBySize & one,
							OmSegmentListBySize & two);

	boost::shared_ptr<OmSegIDsListWithPage>
	GetPageOfSegmentIDs(const unsigned int,
							  const int, const OmSegID);

	int64_t GetSegmentSize(OmSegment* seg);

	size_t Size();
	int64_t VoxelCount(){
		return mVoxels;
	}
	void Dump();
	void Clear();

	OmSegID GetNextSegmentIDinList(const OmSegID id);
	OmSegIDsSet AllSegIDs();

protected:

	void do_incrementSegSize(const OmSegID segID_, const int64_t addedSize);
	void do_decrementSegSize(const OmSegID segID_, const int64_t subbedSize);
	void do_removeSegment(const OmSegID segID_);
	void do_insertSegment(const OmSegID segID_, const int64_t size_);

	struct OmSegSize {
		OmSegID segID;
		int64_t segSize;
		OmSegSize(const OmSegID segID_, const int64_t segSize_)
			: segID(segID_), segSize(segSize_){}
	};

	struct segID{};
	struct segSize{};

	// TODO: change segID to be hash index? (purcaro)
	typedef boost::multi_index_container
	< OmSegSize,
	  boost::multi_index::indexed_by <
		  boost::multi_index::ordered_non_unique<boost::multi_index::tag<segSize>,
												 BOOST_MULTI_INDEX_MEMBER(OmSegSize,int64_t,segSize),
												 std::greater<int64_t> >,
		  boost::multi_index::ordered_unique<boost::multi_index::tag<segID>,
											 BOOST_MULTI_INDEX_MEMBER(OmSegSize,OmSegID,segID) >
		  >
	> OmSegSizes;

	typedef OmSegSizes::index<segSize>::type List_by_size;
	typedef OmSegSizes::index<segID>::type List_by_ID;

	OmSegSizes mList;

	int64_t mVoxels;

private:
	void advanceIter(List_by_size& sizeIndex,
					 List_by_size::iterator& iterSize,
					 const int offset);
};

#endif
