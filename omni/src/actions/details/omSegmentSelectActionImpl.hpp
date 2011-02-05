#ifndef OM_SEGMENT_SELECT_ACTION_IMPL_HPP
#define OM_SEGMENT_SELECT_ACTION_IMPL_HPP

#include "actions/omSelectSegmentParams.hpp"
#include "common/omCommon.h"
#include "common/omString.hpp"
#include "project/omProject.h"
#include "project/omProject.h"
#include "system/omEvents.h"
#include "utility/dataWrappers.h"
#include "utility/omCopyFirstN.hpp"

class OmSegmentSelectActionImpl {
private:
    boost::shared_ptr<OmSelectSegmentsParams> params_;

public:
    OmSegmentSelectActionImpl()
    {}

    OmSegmentSelectActionImpl(boost::shared_ptr<OmSelectSegmentsParams> params)
        : params_(params)
    {}

    void Execute()
    {
        OmSegmentCache* segCache = params_->sdw.SegmentCache();

        if(params_->augmentListOnly){
            if(om::ADD == params_->addOrSubtract){
                segCache->AddToSegmentSelection(params_->newSelectedIDs);
            } else {
                segCache->RemoveFromSegmentSelection(params_->newSelectedIDs);
            }

        }else{
                segCache->UpdateSegmentSelection(params_->newSelectedIDs,
                                                 params_->addToRecentList);
        }

        OmEvents::SegmentModified(params_);
    }

    void Undo()
    {
        OmSegmentCache* segCache = params_->sdw.SegmentCache();

        if(params_->augmentListOnly){
            if(om::ADD == params_->addOrSubtract){
                segCache->RemoveFromSegmentSelection(params_->newSelectedIDs);
            } else {
                segCache->AddToSegmentSelection(params_->newSelectedIDs);
            }

        }else{
            segCache->UpdateSegmentSelection(params_->oldSelectedIDs,
                                             params_->addToRecentList);
        }

        OmEvents::SegmentModified(params_);
    }

    std::string Description() const
    {
        static const int max = 5;

        const std::string nums =
            om::utils::MakeShortStrList<OmSegIDsSet, OmSegID>(params_->newSelectedIDs, max);

        std::string prefix("Selected segments: ");

        if(params_->augmentListOnly){
            if(om::ADD == params_->addOrSubtract){
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
};

#endif
