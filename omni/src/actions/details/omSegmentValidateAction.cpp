#include "actions/io/omActionLogger.hpp"
#include "actions/details/omSegmentValidateAction.h"
#include "actions/details/omSegmentValidateActionImpl.hpp"
#include "segment/omSegmentUtils.hpp"

OmSegmentValidateAction::OmSegmentValidateAction(const SegmentationDataWrapper& sdw,
                                                 om::shared_ptr<std::set<OmSegment*> > selectedSegments,
                                                 const bool valid)
    : impl_(om::make_shared<OmSegmentValidateActionImpl>(sdw,
                                                         selectedSegments,
                                                         valid))
{
    SetUndoable(true);
}

void OmSegmentValidateAction::Action()
{
    impl_->Execute();
}

void OmSegmentValidateAction::UndoAction()
{
    impl_->Undo();
}

std::string OmSegmentValidateAction::Description()
{
    return impl_->Description();
}

void OmSegmentValidateAction::save(const std::string& comment)
{
    OmActionLogger::save(impl_, comment);
}

