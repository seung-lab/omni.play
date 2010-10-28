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
	OmSegmentListBySize(){}

	void insertSegment(OmSegment* seg);
	void removeSegment(OmSegment* seg);
	void updateFromJoin(OmSegment* root, OmSegment* child);
	void updateFromSplit(OmSegment* root, OmSegment* child, const uint64_t);

	static void swapSegment(OmSegment* seg, OmSegmentListBySize & one,
							OmSegmentListBySize & two);

	boost::shared_ptr<OmSegIDsListWithPage>
	getPageOfSegmentIDs(const unsigned int,
							  const int, const OmSegID);

	uint64_t getSegmentSize(OmSegment* seg);

	size_t size();
	void dump();
	void clear();

	OmSegID GetNextSegmentIDinList(const OmSegID id);

protected:

	void do_incrementSegSize(const OmSegID segID_, const uint64_t addedSize);
	void do_removeSegment(const OmSegID segID_);
	void do_insertSegment(const OmSegID segID_, const uint64_t size_);

	struct OmSegSize {
		OmSegID segID;
		uint64_t segSize;
		OmSegSize(const OmSegID segID_, const uint64_t segSize_)
			: segID(segID_), segSize(segSize_){}
	};

	struct segID{};
	struct segSize{};

	// TODO: change segID to be hash index? (purcaro)
	typedef boost::multi_index_container
	< OmSegSize,
	  boost::multi_index::indexed_by <
		  boost::multi_index::ordered_non_unique<boost::multi_index::tag<segSize>,
												 BOOST_MULTI_INDEX_MEMBER(OmSegSize,uint64_t,segSize),
												 std::greater<uint64_t> >,
		  boost::multi_index::ordered_unique<boost::multi_index::tag<segID>,
											 BOOST_MULTI_INDEX_MEMBER(OmSegSize,OmSegID,segID) >
		  >
	> OmSegSizes;

	typedef OmSegSizes::index<segSize>::type List_by_size;
	typedef OmSegSizes::index<segID>::type List_by_ID;

	OmSegSizes mList;

private:
	void advanceIter(List_by_size& sizeIndex,
					 List_by_size::iterator& iterSize,
					 const int offset);
};

#endif
