#include "common/logging.h"
#include "actions/io/omActionLogger.hpp"
#include "actions/details/omSegmentCutAction.h"
#include "actions/details/omSegmentCutActionImpl.hpp"
#include "segment/omFindCommonEdge.hpp"

OmSegmentCutAction::OmSegmentCutAction(const SegmentDataWrapper& sdw)
    : impl_(std::make_shared<OmSegmentCutActionImpl>(sdw))
{
    SetUndoable(true);
}

void OmSegmentCutAction::Action(){
    impl_->Execute();
}

void OmSegmentCutAction::UndoAction(){
    impl_->Undo();
}

std::string OmSegmentCutAction::Description(){
    return impl_->Description();
}

void OmSegmentCutAction::save(const std::string& comment){
    OmActionLogger::save(impl_, comment);
}
