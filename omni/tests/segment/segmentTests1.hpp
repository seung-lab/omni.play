#pragma once

#include "segment/io/omMST.h"
#include "segment/lowLevel/omSegmentsImplLowLevel.h"
#include "segment/lowLevel/omSegmentGraph.h"
#include "segment/omSegments.h"
#include "segment/omSegmentsImpl.h"
#include "segment/omSegmentColorizer.h"
#include "tests/fakeMemMapFile.hpp"
#include "volume/omSegmentation.h"
#include "segment/omSegmentListBySizeTests.hpp"

#include <boost/make_shared.hpp>

class SegmentTests1 {
private:
    boost::shared_ptr<OmSegmentation> seg_;
    OmSegment* s1_;
    OmSegment* s2_;
    OmSegment* s3_;
    OmSegment* s4_;
    OmSegment* s5_;
    OmSegment* s6_;
    OmSegment* s7_;
    OmSegment* s8_;
    OmSegment* s9_;
    OmSegment* s10_;
    OmSegments* cache_;
    std::vector<OmSegment*> segs_;
    static const uint32_t MaxSegID_ = 10;

public:
    SegmentTests1()
        : seg_(boost::make_shared<OmSegmentation>(1))
        , s1_(NULL), s2_(NULL), s3_(NULL), s4_(NULL), s5_(NULL)
        , s6_(NULL), s7_(NULL), s8_(NULL), s9_(NULL), s10_(NULL)
        , cache_(seg_->GetSegmentCache())
    {
        segs_.resize(MaxSegID_+1, NULL);
    }

    void RunAll()
    {
        segmentColor();

        setupAndTestSegments();

        setupMST();

        cache_->refreshTree();

        test1();

        printf("segment tests OK\n");
    }

    void segmentColor()
    {
        segmentColorHelper(0,0,0, 0,0,0);
        segmentColorHelper(5, 19, 83,
                           12, 47, 207);
        segmentColorHelper(1, 2, 3,
                           2, 5, 7);
        segmentColorHelper(250, 250, 250,
                           255, 255, 255);
        segmentColorHelper(101, 101, 101,
                           252, 252, 252);
        segmentColorHelper(102, 102, 102,
                           255, 255, 255);
        segmentColorHelper(100, 101, 102,
                           250, 252, 255);
    }

    void segmentColorHelper(const int r, const int g, const int b,
                            const int rs, const int gs, const int bs)
    {
        const OmColor color = {r, g, b};
        const OmColor colorSelected =
            OmSegmentColorizer::makeSelectedColor(color);
        verify(colorSelected.red == rs);
        verify(colorSelected.green == gs);
        verify(colorSelected.blue == bs);

//		/*
        std::cout << "in color: " << color
                  << ", out color: " << colorSelected
                  << "\n";
//		*/
    }

    void test1()
    {
        seg_->SetDendThreshold(0);

        verify(s10_->getParentSegID() == 0);
        verify(s1_->getParentSegID() == 3);
        verify(s2_->getParentSegID() == 3);
        verify(s3_->getParentSegID() == 5);
        verify(s4_->getParentSegID() == 5);
        verify(s5_->getParentSegID() == 0);
        verify(s6_->getParentSegID() == 0);
        verify(s7_->getParentSegID() == 10);
        verify(s8_->getParentSegID() == 10);
        verify(s9_->getParentSegID() == 10);

        verify(s10_->getChildren().size() == 3);
        verify(s1_->getChildren().size() == 0);
        verify(s2_->getChildren().size() == 0);
        verify(s3_->getChildren().size() == 2);
        verify(s4_->getChildren().size() == 0);
        verify(s5_->getChildren().size() == 2);
        verify(s6_->getChildren().size() == 0);
        verify(s7_->getChildren().size() == 0);
        verify(s8_->getChildren().size() == 0);
        verify(s9_->getChildren().size() == 0);


        verify(s10_->getEdgeNumber() == -1);
        verify(s1_->getEdgeNumber() == 0);
        verify(s2_->getEdgeNumber() == 1);
        verify(s3_->getEdgeNumber() == 6);
        verify(s4_->getEdgeNumber() == 5);
        verify(s5_->getEdgeNumber() == -1);
        verify(s6_->getEdgeNumber() == -1);
        verify(s7_->getEdgeNumber() == 2);
        verify(s8_->getEdgeNumber() == 3);
        verify(s9_->getEdgeNumber() == 4);


        verify(10 == cache_->findRootID(10));
        verify(10 == cache_->findRootID(7));
        verify(10 == cache_->findRootID(8));
        verify(10 == cache_->findRootID(9));
        verify(5 == cache_->findRootID(1));
        verify(5 == cache_->findRootID(2));
        verify(5 == cache_->findRootID(3));
        verify(5 == cache_->findRootID(4));
        verify(5 == cache_->findRootID(5));
        verify(6 == cache_->findRootID(6));

        //OmMSTEdge* edges = seg_->getMST()->Edges();
    }

private:
    void setupAndTestSegments()
    {
        verify(cache_);

        verify(false == cache_->IsSegmentValid(0));
        verify(false == cache_->IsSegmentValid(11));
        segs_[1] = s1_ = cache_->GetOrAddSegment(1);
        segs_[2] = s2_ = cache_->GetOrAddSegment(2);
        segs_[3] = s3_ = cache_->GetOrAddSegment(3);
        segs_[4] = s4_ = cache_->GetOrAddSegment(4);
        segs_[5] = s5_ = cache_->GetOrAddSegment(5);
        segs_[6] = s6_ = cache_->GetOrAddSegment(6);
        segs_[7] = s7_ = cache_->GetOrAddSegment(7);
        segs_[8] = s8_ = cache_->GetOrAddSegment(8);
        segs_[9] = s9_ = cache_->GetOrAddSegment(9);
        segs_[10] = s10_ = cache_->GetOrAddSegment(10);

        if(MaxSegID_ > 10){
            for(uint32_t i = 11; i < MaxSegID_+1; ++i){
                segs_[i] = cache_->GetOrAddSegment(i);
            }
        }

        for(uint32_t i = 1; i < MaxSegID_+1; ++i){
            verify(segs_[i]);
            verify(segs_[i]->value() == i);
            verify(segs_[i]->getBounds().isEmpty());
        }

        OmSegment* dup = cache_->GetOrAddSegment(10);
        verify(dup == s10_);
    }

    void setupMST()
    {
        boost::shared_ptr<OmMST> mst(new OmMST(seg_.get()));
        const uint32_t numEdges = 7;
        mst->edgesPtr_ =
            boost::make_shared<FakeMemMapFile<OmMSTEdge> >("fake MST", numEdges);

        mst->numEdges_ = numEdges;
        mst->edges_ = mst->edgesPtr_->GetPtr();
        OmMSTEdge* edges = mst->Edges();

        for(uint32_t i = 0; i < numEdges; ++i){
            edges[i].number = i;
            edges[i].userSplit = 0;
            edges[i].userJoin = 0;
            edges[i].wasJoined = 0;
        }

        edges[0].node1ID = 1;
        edges[0].node2ID = 3;
        edges[0].threshold = .95;

        edges[1].node1ID = 2;
        edges[1].node2ID = 3;
        edges[1].threshold = .9;

        edges[2].node1ID = 7;
        edges[2].node2ID = 10;
        edges[2].threshold = .85;

        edges[3].node1ID = 8;
        edges[3].node2ID = 10;
        edges[3].threshold = .7;

        edges[4].node1ID = 9;
        edges[4].node2ID = 10;
        edges[4].threshold = .6;

        edges[5].node1ID = 4;
        edges[5].node2ID = 5;
        edges[5].threshold = .5;

        edges[6].node1ID = 3;
        edges[6].node2ID = 5;
        edges[6].threshold = .4;

        seg_->mst_ = mst;
    }
};
