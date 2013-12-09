#include "common/logging.h"
#include "actions/io/omActionLogger.hpp"
#include "actions/details/omSegmentShatterAction.h"
#include "actions/details/omSegmentShatterActionImpl.hpp"
#include "segment/omFindCommonEdge.hpp"

OmSegmentShatterAction::OmSegmentShatterAction(const SegmentDataWrapper& sdw)
    : impl_(std::make_shared<OmSegmentShatterActionImpl>(sdw)) {
  SetUndoable(true);
}

void OmSegmentShatterAction::Action() { impl_->Execute(); }

void OmSegmentShatterAction::UndoAction() { impl_->Undo(); }

std::string OmSegmentShatterAction::Description() {
  return impl_->Description();
}

void OmSegmentShatterAction::save(const std::string& comment) {
  OmActionLogger::save(impl_, comment);
}
