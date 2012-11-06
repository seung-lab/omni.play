#pragma once

#include "actions/details/omActionBase.hpp"

#include "actions/details/omProjectCloseActionImpl.hpp"
class OmProjectCloseAction : public OmActionBase<OmProjectCloseActionImpl>{
public:
    OmProjectCloseAction(){
        SetUndoable(false);
    }
};

#include "actions/details/omProjectSaveActionImpl.hpp"
class OmProjectSaveAction : public OmActionBase<OmProjectSaveActionImpl>{
public:
    OmProjectSaveAction(){
        SetUndoable(false);
    }
};

#include "actions/details/omSegmentationThresholdChangeActionImpl.hpp"
class OmSegmentationThresholdChangeAction
    : public OmActionBase<OmSegmentationThresholdChangeActionImpl>{
public:
    OmSegmentationThresholdChangeAction(boost::shared_ptr<OmSegmentationThresholdChangeActionImpl> impl)
    {
        impl_ = impl;
    }

    OmSegmentationThresholdChangeAction(const SegmentationDataWrapper sdw,
                                        const double threshold)
    {
        impl_ = boost::make_shared<OmSegmentationThresholdChangeActionImpl>(sdw, threshold);
        SetUndoable(true);
    }
};

#include "actions/details/omSegmentationSizeThresholdChangeActionImpl.hpp"
class OmSegmentationSizeThresholdChangeAction
    : public OmActionBase<OmSegmentationSizeThresholdChangeActionImpl>{
public:
    OmSegmentationSizeThresholdChangeAction(boost::shared_ptr<OmSegmentationSizeThresholdChangeActionImpl> impl)
    {
        impl_ = impl;
    }
    
    OmSegmentationSizeThresholdChangeAction(const SegmentationDataWrapper sdw,
                                            const double threshold)
    {
        impl_ = boost::make_shared<OmSegmentationSizeThresholdChangeActionImpl>(sdw, threshold);
        SetUndoable(true);
    }
};

#include "actions/details/omSegmentSelectActionImpl.hpp"
class OmSegmentSelectAction : public OmActionBase<OmSegmentSelectActionImpl>{
public:
    OmSegmentSelectAction(boost::shared_ptr<OmSegmentSelectActionImpl> impl)
    {
        impl_ = impl;
    }

    OmSegmentSelectAction(boost::shared_ptr<OmSelectSegmentsParams> params)
    {
        impl_ = boost::make_shared<OmSegmentSelectActionImpl>(params);
    }
};

#include "actions/details/omSegmentJoinActionImpl.hpp"
class OmSegmentJoinAction : public OmActionBase<OmSegmentJoinActionImpl>{
public:
    OmSegmentJoinAction(boost::shared_ptr<OmSegmentJoinActionImpl> impl)
    {
        impl_ = impl;
    }

    OmSegmentJoinAction(const SegmentationDataWrapper& sdw,
                        const om::common::SegIDSet& ids)
    {
        impl_ = boost::make_shared<OmSegmentJoinActionImpl>(sdw, ids);
        SetUndoable(true);
    }

    virtual void Action()
    {
        impl_->Execute();
        SetDescription();
    }

    virtual void UndoAction()
    {
        impl_->Undo();
        SetDescription();
        OmEvents::Redraw2d();
    }
};

#include "actions/details/omSegmentGroupActionImpl.hpp"
class OmSegmentGroupAction : public OmActionBase<OmSegmentGroupActionImpl>{
public:
    OmSegmentGroupAction(boost::shared_ptr<OmSegmentGroupActionImpl> impl)
    {
        impl_ = impl;
    }

    OmSegmentGroupAction(const om::common::ID segmentationId,
                         const om::common::SegIDSet& selectedSegmentIds,
                         const om::common::GroupName name,
                         const bool create)
    {
        impl_ = boost::make_shared<OmSegmentGroupActionImpl>(segmentationId,
                                                             selectedSegmentIds,
                                                             name,
                                                             create);
        SetUndoable(true);
    }
};

