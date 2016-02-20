#include "events/details/preferenceEvent.h"
#include "events/details/segmentEvent.h"
#include "events/details/view2dEvent.h"
#include "events/details/view3dEvent.h"
#include "segment/omSegmentCenter.hpp"
#include "system/omPreferences.h"
#include "view3d/camera.h"
#include "view3d/omniEventListener.h"
#include "view3d/view3d.h"
#include "viewGroup/omViewGroupState.h"
#include "viewGroup/omViewGroupView2dState.hpp"

namespace om {
namespace v3d {

void OmniEventListener::SegmentModificationEvent(om::event::SegmentEvent*) {
  view3d_.TimedUpdate();
}

void OmniEventListener::ViewBoxChangeEvent() { view3d_.TimedUpdate(); }

void OmniEventListener::View3dRedrawEvent() { view3d_.TimedUpdate(); }

void OmniEventListener::View3dRedrawEventFromCache() { view3d_.TimedUpdate(); }

void OmniEventListener::View3dRecenter() {
  boost::optional<float> distance =
      OmSegmentCenter::ComputeCameraDistanceForSelectedSegments();
  if (!distance) {
    distance = 100.0f;
  }

  auto& camera = view3d_.GetCamera();

  camera.SetDistance(*distance);
  auto globalCoord = vgs_.View2dState().GetScaledSliceDepth();
  camera.SetFocus(globalCoord);

  view3d_.TimedUpdate();
}

/*
 *  Calls function for relevant preferences and redraws,
 *  otherwise event is ignored.
 */
void OmniEventListener::PreferenceChangeEvent(event::PreferenceEvent* e) {
  view3d_.UpdatePreferences();
  view3d_.TimedUpdate();
}
}
}  // om::v3d
