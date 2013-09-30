#include "segment/omSegmentUtils.hpp"
#include "actions/io/omActionLogger.hpp"
#include "actions/details/omSegmentUncertainAction.h"
#include "actions/details/omSegmentUncertainActionImpl.hpp"

OmSegmentUncertainAction::OmSegmentUncertainAction(
    const SegmentationDataWrapper& sdw,
    std::shared_ptr<std::set<OmSegment*> > selectedSegments,
    const bool uncertain)
    : impl_(std::make_shared<OmSegmentUncertainActionImpl>(sdw,
                                                           selectedSegments,
                                                           uncertain)) {
  SetUndoable(true);
}

/////////////////////////////////
///////          Action Methods
void OmSegmentUncertainAction::Action() { impl_->Execute(); }

void OmSegmentUncertainAction::UndoAction() { impl_->Undo(); }

std::string OmSegmentUncertainAction::Description() {
  return impl_->Description();
}

void OmSegmentUncertainAction::save(const std::string& comment) {
  OmActionLogger::save(impl_, comment);
}
