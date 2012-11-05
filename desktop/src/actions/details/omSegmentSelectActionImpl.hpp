#pragma once

#include "actions/omSelectSegmentParams.hpp"
#include "common/common.h"
#include "common/omString.hpp"
#include "project/omProject.h"
#include "project/omProject.h"
#include "events/omEvents.h"
#include "utility/dataWrappers.h"
#include "utility/omCopyFirstN.hpp"

class OmSegmentSelectActionImpl {
private:
    om::shared_ptr<OmSelectSegmentsParams> params_;

public:
    OmSegmentSelectActionImpl()
    {}

    OmSegmentSelectActionImpl(om::shared_ptr<OmSelectSegmentsParams> params)
        : params_(params)
    {}

    void Execute()
    {
    	OmSegments* segments = params_->sdw.Segments();

        if(params_->augmentListOnly)
		{
            if(om::common::ADD == params_->addOrSubtract){
                segments->AddToSegmentSelection(params_->newSelectedIDs);

            } else {
                segments->RemoveFromSegmentSelection(params_->newSelectedIDs);
            }

        }else{
                segments->UpdateSegmentSelection(params_->newSelectedIDs,
                                                 params_->addToRecentList);
        }

        OmEvents::SegmentModified(params_);
    }

    void Undo()
    {
        OmSegments* segments = params_->sdw.Segments();

        if(params_->augmentListOnly)
		{
            if(om::common::ADD == params_->addOrSubtract){
                segments->RemoveFromSegmentSelection(params_->newSelectedIDs);

            } else {
                segments->AddToSegmentSelection(params_->newSelectedIDs);
            }

        }else{
            segments->UpdateSegmentSelection(params_->oldSelectedIDs,
                                             params_->addToRecentList);
        }

        OmEvents::SegmentModified(params_);
    }

    std::string Description() const
    {
        static const int max = 5;

        const std::string nums =
            om::utils::MakeShortStrList<om::common::SegIDSet, om::common::SegID>(params_->newSelectedIDs, max);

        std::string prefix("Selected segments: ");

        if(params_->augmentListOnly){
            if(om::common::ADD == params_->addOrSubtract){
                prefix = "Added segments: ";
            }else{
                prefix = "Removed segments: ";
            }
        }

        return prefix + nums;
    }

    QString classNameForLogFile() const {
        return "OmSegmentSelectAction";
    }

private:
    template <typename T> friend class OmActionLoggerThread;
    friend QDataStream &operator<<(QDataStream&, const OmSegmentSelectActionImpl&);
    friend QDataStream &operator>>(QDataStream&, OmSegmentSelectActionImpl&);

    friend QTextStream& operator<<(QTextStream& out, const OmSegmentSelectActionImpl&);
};

