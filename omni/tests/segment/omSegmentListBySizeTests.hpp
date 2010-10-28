#ifndef OM_SEGMENT_LIST_BY_SIZE_TESTS_HPP
#define OM_SEGMENT_LIST_BY_SIZE_TESTS_HPP

#include "segment/lowLevel/omSegmentListBySize.h"
#include "segment/mockSegments.hpp"

class OmSegmentListBySizeTests {
public:
	OmSegmentListBySizeTests()
		:segs_(boost::make_shared<MockSegments>())
	{}

	void RunAll()
	{
		run1();
	}

private:
	boost::shared_ptr<MockSegments> segs_;

	void run1()
	{
		OmSegmentListBySize list;
		MockSegments& segs = *segs_;

		for( int i = 1; i <= 10; i++){
			segs[i]->addToSize(i);
			list.InsertSegment(segs[i]);
		}

		verify(10 == list.Size());

		list.UpdateFromJoin(segs[2], segs[1]);
		verify( 9 == list.Size());
		verify( 3 == list.GetSegmentSize(segs[2]));

		list.UpdateFromSplit(segs[2], segs[1], 1);
		verify(10 == list.Size());
		verify( 2 == list.GetSegmentSize(segs[2]));

		boost::shared_ptr<OmSegIDsListWithPage> page =
			list.GetPageOfSegmentIDs(0, 3, 0);

		OmSegIDsList& pageList1 = *page->List();
 		verify( 10 == pageList1[0] );
 		verify(  9 == pageList1[1] );
 		verify(  8 == pageList1[2] );

// join 9 into 10 and check page
		list.UpdateFromJoin(segs[10], segs[9]);
		verify( 9 == list.Size());
		verify( 19 == list.GetSegmentSize(segs[10]));

		page = list.GetPageOfSegmentIDs(0, 3, 0);
		OmSegIDsList& pageList2 = *page->List();
 		verify( 10 == pageList2[0] );
 		verify(  8 == pageList2[1] );
 		verify(  7 == pageList2[2] );

		list.Dump();
	}
};

#endif
