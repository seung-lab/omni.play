#include "actions/omSelectSegmentParams.hpp"
#include "events/details/segmentEvent.h"
#include "common/logging.h"

namespace om {
namespace event {

const QEvent::Type SegmentEvent::MODIFIED =
    (QEvent::Type) QEvent::registerEventType();
const QEvent::Type SegmentEvent::SEGMENT_GUI_LIST =
    (QEvent::Type) QEvent::registerEventType();
const QEvent::Type SegmentEvent::SEGMENT_SELECTED =
    (QEvent::Type) QEvent::registerEventType();

SegmentEvent::SegmentEvent(QEvent::Type type)
    : Event(Klass::segment, type),
      params_(std::make_shared<OmSelectSegmentsParams>()) {}

SegmentEvent::SegmentEvent(QEvent::Type type,
                           const SegmentationDataWrapper& sdw,
                           const bool stayOnPage)
    : Event(Klass::segment, type), guiParams_(new OmSegmentGUIparams()) {
  guiParams_->sdw = sdw;
  guiParams_->stayOnPage = stayOnPage;
}

SegmentEvent::SegmentEvent(QEvent::Type type,
                           std::shared_ptr<OmSelectSegmentsParams> params)
    : Event(Klass::segment, type), params_(params) {}

void SegmentEvent::Dispatch(Listener* base) {
  auto* list = dynamic_cast<SegmentEventListener*>(base);
  assert(list);
  if (!list) {
    log_debugs << "No listeners found";
    return;
  }

  if (type_ == MODIFIED) return list->SegmentModificationEvent(this);
  if (type_ == SEGMENT_GUI_LIST) return list->SegmentGUIlistEvent(this);
  if (type_ == SEGMENT_SELECTED) return list->SegmentSelectedEvent(this);
  throw om::ArgException("unknown event type");
}
}
}
