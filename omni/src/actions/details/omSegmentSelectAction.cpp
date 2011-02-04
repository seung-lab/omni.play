#include "actions/io/omActionLogger.hpp"
#include "actions/details/omSegmentSelectAction.h"
#include "actions/details/omSegmentSelectActionImpl.hpp"

OmSegmentSelectAction::OmSegmentSelectAction(boost::shared_ptr<OmSelectSegmentsParams> params)
    : impl_(boost::make_shared<OmSegmentSelectActionImpl>(params))
{}

void OmSegmentSelectAction::Action(){
    impl_->Execute();
}

void OmSegmentSelectAction::UndoAction(){
    impl_->Undo();
}

std::string OmSegmentSelectAction::Description(){
    return impl_->Description();
}

void OmSegmentSelectAction::save(const std::string& comment){
    OmActionLogger::save(impl_, comment);
}
