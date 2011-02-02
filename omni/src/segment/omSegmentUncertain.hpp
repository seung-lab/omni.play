#ifndef OM_SEGMENT_UNCERTAIN_HPP
#define OM_SEGMENT_UNCERTAIN_HPP

#include "common/omCommon.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentLists.hpp"
#include "utility/dataWrappers.h"
#include "utility/omTimer.hpp"
#include "volume/omSegmentation.h"
#include "zi/omMutex.h"

class OmSegmentUncertain {
public:
	static void SetAsUncertain(const SegmentationDataWrapper& sdw,
							   boost::shared_ptr<std::set<OmSegment*> > selectedSegments,
							   const bool uncertain)
	{
		OmSegmentUncertain uncertainer(sdw, selectedSegments, uncertain);
		uncertainer.setAsUncertain();
	}

private:
	const SegmentationDataWrapper& sdw_;
	const boost::shared_ptr<std::set<OmSegment*> > selectedSegments_;
	const bool uncertain_;
	const boost::shared_ptr<OmSegmentLists> segmentLists_;

	OmSegmentUncertain(const SegmentationDataWrapper& sdw,
					   boost::shared_ptr<std::set<OmSegment*> > selectedSegments,
					   const bool uncertain)
		: sdw_(sdw)
		, selectedSegments_(selectedSegments)
		, uncertain_(uncertain)
		, segmentLists_(sdw_.SegmentLists())
	{}

	void setAsUncertain()
	{
		static zi::mutex mutex;
		zi::guard g(mutex);

		OmTimer timer;

		if(uncertain_){
			std::cout << "setting " << selectedSegments_->size()
					  << " segments as uncertain..." << std::flush;
		} else {
			std::cout << "setting " << selectedSegments_->size()
					  << " segments as NOT uncertain..." << std::flush;
		}

		FOR_EACH(iter, *selectedSegments_){
			OmSegment* seg = *iter;

			if(uncertain_){
				segmentLists_->MoveSegment(om::UNCERTAIN, seg);
			} else {
				segmentLists_->MoveSegment(om::WORKING, seg);
			}
		}

		printf("done (%.2g secs)\n", timer.s_elapsed());
	}
};

#endif
