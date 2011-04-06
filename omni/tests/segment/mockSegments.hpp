#ifndef MOCK_SEGMENTS_HPP
#define MOCK_SEGMENTS_HPP

#include "segment/io/omMST.h"
#include "segment/lowLevel/omSegmentsImplLowLevel.h"
#include "segment/lowLevel/omSegmentGraph.h"
#include "segment/omSegments.h"
#include "segment/omSegmentsImpl.h"
#include "segment/colorizer/omSegmentColorizer.h"
#include "tests/fakeMemMapFile.hpp"
#include "volume/omSegmentation.h"
#include "tests/testUtils.hpp"

class MockSegments {
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
    OmSegments* segments_;
    std::vector<OmSegment*> segs_;
    static const uint32_t MaxSegID_ = 10;

    // ensure unique set of segments everything we instantiate a new
    //  MockSegments object
    static OmID segmentationID;

public:
    MockSegments()
        : seg_(boost::make_shared<OmSegmentation>(segmentationID++))
        , s1_(NULL), s2_(NULL), s3_(NULL), s4_(NULL), s5_(NULL)
        , s6_(NULL), s7_(NULL), s8_(NULL), s9_(NULL), s10_(NULL)
        , segments_(seg_->Segments())
    {
        segs_.resize(MaxSegID_+1, NULL);
        setupAndTestSegments();
    }

    OmSegment* GetOrAddSegment(const uint32_t id)
    {
        OmSegment* seg = segments_->GetOrAddSegment(id);
        if(segs_.size() <= id){
            segs_.resize(id+1);
        }
        segs_[id] = seg;
        return seg;
    }

    OmSegment* operator[] (const uint32_t id){
        return segs_[id];
    }

private:
    void setupAndTestSegments()
    {
        verify(segments_);

        verify(false == segments_->IsSegmentValid(0));
        verify(false == segments_->IsSegmentValid(11));
        segs_[1] = s1_ = segments_->GetOrAddSegment(1);
        segs_[2] = s2_ = segments_->GetOrAddSegment(2);
        segs_[3] = s3_ = segments_->GetOrAddSegment(3);
        segs_[4] = s4_ = segments_->GetOrAddSegment(4);
        segs_[5] = s5_ = segments_->GetOrAddSegment(5);
        segs_[6] = s6_ = segments_->GetOrAddSegment(6);
        segs_[7] = s7_ = segments_->GetOrAddSegment(7);
        segs_[8] = s8_ = segments_->GetOrAddSegment(8);
        segs_[9] = s9_ = segments_->GetOrAddSegment(9);
        segs_[10] = s10_ = segments_->GetOrAddSegment(10);

        if(MaxSegID_ > 10)
        {
            for(uint32_t i = 11; i < MaxSegID_+1; ++i){
                segs_[i] = segments_->GetOrAddSegment(i);
            }
        }

        for(uint32_t i = 1; i < MaxSegID_+1; ++i)
        {
            verify(segs_[i]);
            verify(segs_[i]->value() == i);
            verify(segs_[i]->BoundingBox().isEmpty());
        }

        OmSegment* dup = segments_->GetOrAddSegment(10);
        verify(dup == s10_);
    }
};

#endif
