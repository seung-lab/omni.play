#pragma once
#include "precomp.h"

#include "events/details/event.h"

class OmSegmentAction;
class OmSelectSegmentsParams;
class SegmentationDataWrapper;
class OmSegmentGUIparams;

namespace om {
namespace event {

class SegmentEvent : public Event {
 public:
  static const QEvent::Type MODIFIED;
  static const QEvent::Type SEGMENT_GUI_LIST;
  static const QEvent::Type SEGMENT_SELECTED;
  static const QEvent::Type SEGMENT_BRUSH;


  SegmentEvent(QEvent::Type type);
  SegmentEvent(QEvent::Type, const SegmentationDataWrapper&, const bool);
  SegmentEvent(QEvent::Type, std::shared_ptr<OmSelectSegmentsParams>);

  void Dispatch(Listener*);

  inline const OmSelectSegmentsParams& Params() { return *params_; }

  inline const OmSegmentGUIparams& GUIparams() { return *guiParams_; }

 private:
  std::shared_ptr<OmSelectSegmentsParams> params_;
  std::unique_ptr<OmSegmentGUIparams> guiParams_;
};
}
}  // om::event::
