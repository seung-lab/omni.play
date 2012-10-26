#pragma once

#include "common/common.h"
#include "segment/omSegments.h"
#include "segment/lists/omSegmentLists.h"
#include "utility/dataWrappers.h"
#include "utility/omTimer.hpp"
#include "volume/omSegmentation.h"
#include "zi/omMutex.h"

class OmSegmentUncertain {
public:
    static void SetAsUncertain(const SegmentationDataWrapper& sdw,
                               om::shared_ptr<std::set<OmSegment*> > selectedSegments,
                               const bool uncertain)
    {
        OmSegmentUncertain uncertainer(sdw, selectedSegments, uncertain);
        uncertainer.setAsUncertain();
    }

private:
    const SegmentationDataWrapper& sdw_;
    const om::shared_ptr<std::set<OmSegment*> > selectedSegments_;
    const bool uncertain_;

    OmSegmentUncertain(const SegmentationDataWrapper& sdw,
                       om::shared_ptr<std::set<OmSegment*> > selectedSegments,
                       const bool uncertain)
        : sdw_(sdw)
        , selectedSegments_(selectedSegments)
        , uncertain_(uncertain)
    {}

    void setAsUncertain()
    {
        static zi::mutex mutex;
        zi::guard g(mutex);

        OmTimer timer;

        if(uncertain_)
        {
            std::cout << "setting " << selectedSegments_->size()
                      << " segments as uncertain..." << std::flush;
        } else
        {
            std::cout << "setting " << selectedSegments_->size()
                      << " segments as NOT uncertain..." << std::flush;
        }

        FOR_EACH(iter, *selectedSegments_)
        {
            OmSegment* seg = *iter;

            if(uncertain_){
                seg->SetListType(om::UNCERTAIN);
            } else {
                seg->SetListType(om::WORKING);
            }
        }

        sdw_.SegmentLists()->RefreshGUIlists();

        timer.PrintDone();
    }
};

