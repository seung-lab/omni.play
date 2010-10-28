#ifndef MOCK_SEGMENTS_HPP
#define MOCK_SEGMENTS_HPP

#include "segment/io/omMST.h"
#include "segment/lowLevel/omSegmentCacheImplLowLevel.h"
#include "segment/lowLevel/omSegmentGraph.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentCacheImpl.h"
#include "segment/omSegmentColorizer.h"
#include "tests/fakeMemMapFile.hpp"
#include "volume/omSegmentation.h"

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
	OmSegmentCache* cache_;
	std::vector<OmSegment*> segs_;
	static const uint32_t MaxSegID_ = 10;

public:
	MockSegments()
		: seg_(boost::make_shared<OmSegmentation>(1))
		, s1_(NULL), s2_(NULL), s3_(NULL), s4_(NULL), s5_(NULL)
		, s6_(NULL), s7_(NULL), s8_(NULL), s9_(NULL), s10_(NULL)
		, cache_(seg_->GetSegmentCache())
	{
		segs_.resize(MaxSegID_+1, NULL);
		setupAndTestSegments();
	}

	OmSegment* operator[] (const uint32_t id){
		return segs_[id];
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
};

#endif
