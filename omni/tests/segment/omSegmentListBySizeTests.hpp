#ifndef OM_SEGMENT_LIST_BY_SIZE_TESTS_HPP
#define OM_SEGMENT_LIST_BY_SIZE_TESTS_HPP

#include "utility/omTimer.hpp"
#include "segment/mockSegments.hpp"
#include "segment/omSegment.h"

class OmSegmentListBySizeTests {
public:
    OmSegmentListBySizeTests()
    {}

    void RunAll()
    {
        assert(0 && "please fix me");
/*
        runBasicListTests<OmSegmentListBySize>();
        runBasicListTests<OmSegmentListBySize2>();

        testMultipleSize<OmSegmentListBySize2>();

        //runLargeNumSegmentsTest<OmSegmentListBySize>();
        runLargeNumSegmentsTest<OmSegmentListBySize2>();
*/
    }

private:
/*
    template <typename T>
    void runBasicListTests()
    {
        T list;
        MockSegments segs;

        for( int i = 1; i <= 10; ++i){
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

        {
            boost::shared_ptr<OmSegIDsListWithPage> page =
                list.GetPageOfSegmentIDs(0, 3, 0);
            OmSegIDsList& pageList = *page->List();
            verify( 10 == pageList[0] );
            verify(  9 == pageList[1] );
            verify(  8 == pageList[2] );
        }

// join 9 into 10 and check page
        list.UpdateFromJoin(segs[10], segs[9]);
        verify( 9 == list.Size());
        verify( 19 == list.GetSegmentSize(segs[10]));

        {
            boost::shared_ptr<OmSegIDsListWithPage> page =
                list.GetPageOfSegmentIDs(0, 3, 0);
            OmSegIDsList& pageList = *page->List();
            verify( 10 == pageList[0] );
            verify(  8 == pageList[1] );
            verify(  7 == pageList[2] );
        }

        list.Dump();

        list.Clear();
        assert( 0 == list.Size());
    }
*/
/*

  sample run:

  no graph found...
  99999
  done in 1.97 secs
  getting pages...
  99999
  done in 845.61 secs
  [Thread 0x7fffe97fa910 (LWP 17816) exited]
  [New Thread 0x7fffe9ffb910 (LWP 17995)]
  [Thread 0x7fffe87f8910 (LWP 17817) exited]
  [New Thread 0x7fffe97fa910 (LWP 17996)]
  [Thread 0x7fffe8ff9910 (LWP 17818) exited]
  [New Thread 0x7fffe87f8910 (LWP 17997)]
  no graph found...
  99999
  done in 8.98 secs
  getting pages...
  99999
  done in 0.82 secs
*/
/*
    template <typename T>
    void runLargeNumSegmentsTest()
    {
        const uint64_t maxN = 100000;

        T list;
        MockSegments segs;

        for(uint64_t i = 1; i < maxN; ++i){
            segs.GetOrAddSegment(i);
            segs[i]->addToSize(i);
        }

        {
            OmTimer timer;
            for(uint64_t i = 1; i < maxN; ++i){
                std::cout << "\r\t" << i << std::flush;
                list.InsertSegment(segs[i]);
            }

            printf("\n");
            timer.PrintDone();
        }

        {
            const int pageSize = 100;
            printf("getting pages...\n");
            OmTimer timer;
            for(uint64_t i = 0; i < maxN; ++i){
                std::cout << "\r\t" << i << std::flush;
                boost::shared_ptr<OmSegIDsListWithPage> page =
                    list.GetPageOfSegmentIDs(i, pageSize, 0);
            }

            printf("\n");
            timer.PrintDone();
        }
    }

    // segments that are the same size must be ordered by increasing
    //  segment ID
    template <typename T>
    void testMultipleSize()
    {
        T list;
        MockSegments segs;
        const uint64_t maxN = 1000;

        for(uint64_t i = maxN-1; i > 0; --i){
            segs.GetOrAddSegment(i);
            segs[i]->addToSize(100);
            list.InsertSegment(segs[i]);
        }

        for(uint64_t i = maxN; i < 2*maxN; ++i){
            segs.GetOrAddSegment(i);
            segs[i]->addToSize(100);
            list.InsertSegment(segs[i]);
        }

        {
            const uint64_t numEle = 2*maxN - 1; // seg id 0 not allowed
            boost::shared_ptr<OmSegIDsListWithPage> page =
                list.GetPageOfSegmentIDs(0, numEle, 0);
            OmSegIDsList& pageList = *page->List();
            verify(numEle == pageList.size());
            verify( 1 == pageList[0] );
            verify( numEle == pageList[numEle-1] );
        }
    }
*/
    //TODO: test for multiple inserts of same segment??
};

#endif
