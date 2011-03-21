#ifndef OM_SEGMENT_VALIDATION_HPP
#define OM_SEGMENT_VALIDATION_HPP

#include "common/omCommon.h"
#include "segment/io/omMST.h"
#include "segment/omSegments.h"
#include "segment/lists/omSegmentLists.h"
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
    OmMSTEdge *const edges_;

    OmSegmentValidation(const SegmentationDataWrapper& sdw,
                        boost::shared_ptr<std::set<OmSegment*> > selectedSegments,
                        const bool valid)
        : sdw_(sdw)
        , selectedSegments_(selectedSegments)
        , valid_(valid)
        , edges_(sdw_.MST()->Edges())
    {}

    void setAsValidated()
    {
        static zi::mutex mutex;
        zi::guard g(mutex);

        OmTimer timer;

        std::string notValid("");
        if(!valid_){
            notValid = " NOT";
        }

        std::cout << "setting " << selectedSegments_->size()
                  << " segments as" << notValid << " valid..." << std::flush;

        FOR_EACH(iter, *selectedSegments_)
        {
            OmSegment* seg = *iter;

            if(valid_){
                seg->SetListType(om::VALID);
            } else {
                seg->SetListType(om::WORKING);
            }

            setSegEdge(seg);
        }

        sdw_.ValidGroupNum()->Set(selectedSegments_, valid_);

        sdw_.SegmentLists()->RefreshGUIlists();

        timer.PrintDone();
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
