#pragma once

#include "events/listeners.h"

class OmViewGroupState;

namespace om {
namespace v3d {

class View3d;

class OmniEventListener : public om::event::PreferenceEventListener,
                          public om::event::SegmentEventListener,
                          public om::event::View2dEventListener,
                          public om::event::View3dEventListener {
 private:
  View3d& view3d_;
  OmViewGroupState& vgs_;

 public:
  OmniEventListener(View3d& view3d, OmViewGroupState& vgs)
      : view3d_(view3d), vgs_(vgs) {}

  // omni events
  void SegmentModificationEvent(om::event::SegmentEvent*);
  void SegmentGUIlistEvent(om::event::SegmentEvent*) {}
  void SegmentSelectedEvent(om::event::SegmentEvent*) {}
  void PreferenceChangeEvent(om::event::PreferenceEvent*);
  void ViewBoxChangeEvent();
  void View3dRedrawEvent();
  void View3dRedrawEventFromCache();
  void ViewCenterChangeEvent() {}
  void View3dRecenter();
  void ViewPosChangeEvent() {}
  void ViewRedrawEvent() {}
  void ViewBlockingRedrawEvent() {}
  void CoordSystemChangeEvent() {}
};
}
}  // om::v3d::
