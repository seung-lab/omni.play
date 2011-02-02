#ifndef OM_SEGMENT_VALIDATION_HPP
#define OM_SEGMENT_VALIDATION_HPP

#include "common/omCommon.h"
#include "segment/io/omMST.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentLists.hpp"
#include "utility/dataWrappers.h"
#include "utility/omTimer.hpp"
#include "volume/omSegmentation.h"
#include "zi/omMutex.h"
#include "segment/io/omValidGroupNum.hpp"

class OmSegmentValidation {
public:
	static void SetAsValidated(const SegmentationDataWrapper& sdw,
							   boost::shared_ptr<std::set<OmSegment*> > selectedSegments,
							   const bool valid)
	{
		OmSegmentValidation validator(sdw, selectedSegments, valid);
		validator.setAsValidated();
	}

private:
	const SegmentationDataWrapper& sdw_;
	const boost::shared_ptr<std::set<OmSegment*> > selectedSegments_;
	const bool valid_;
	const boost::shared_ptr<OmSegmentLists> segmentLists_;
	OmMSTEdge *const edges_;

	OmSegmentValidation(const SegmentationDataWrapper& sdw,
						boost::shared_ptr<std::set<OmSegment*> > selectedSegments,
						const bool valid)
		: sdw_(sdw)
		, selectedSegments_(selectedSegments)
		, valid_(valid)
		, segmentLists_(sdw_.SegmentLists())
		, edges_(sdw_.MST()->Edges())
	{}

	void setAsValidated()
	{
		static zi::mutex mutex;
		zi::guard g(mutex);

		OmTimer timer;

		if(valid_){
			std::cout << "setting " << selectedSegments_->size()
					  << " segments as valid..." << std::flush;
		} else {
			std::cout << "setting " << selectedSegments_->size()
					  << " segments as NOT valid..." << std::flush;
		}

		FOR_EACH(iter, *selectedSegments_){
			OmSegment* seg = *iter;

			if(valid_){
				segmentLists_->MoveSegment(om::VALID, seg);
			} else {
				segmentLists_->MoveSegment(om::WORKING, seg);
			}
			setSegEdge(seg);
		}

		sdw_.ValidGroupNum()->Set(sdw_, selectedSegments_, valid_);

		printf("done (%.2g secs)\n", timer.s_elapsed());
	}

	inline void setSegEdge(OmSegment* seg)
	{
		const int edgeNum = seg->getEdgeNumber();
		if( -1 == edgeNum ){
			return;
		}

		// force edge to be joined on MST load
		edges_[edgeNum].userJoin = valid_;
	}
};

#endif
