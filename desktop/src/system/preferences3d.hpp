#pragma once
#include "precomp.h"

#include "events/events.h"

namespace om {
namespace prefs {

struct View3d {
  bool ANTIALIASING;
  Vector3f BACKGROUND_COLOR;
  float CAMERA_FOV;
  float CAMERA_NEAR_PLANE;
  int FOCUS_STYLE;
  bool SHOW_AXIS;
  bool SHOW_CHUNK_EXTENT;
  bool SHOW_INFO;
  bool SHOW_FOCUS;
  bool SHOW_VIEWBOX;

  View3d() { Reset(); }

  void Reset() {
    ANTIALIASING = false;
    BACKGROUND_COLOR = Vector3f::ZERO;
    CAMERA_FOV = 60.0f;
    CAMERA_NEAR_PLANE = 1.0f;
    FOCUS_STYLE = 1;
    SHOW_AXIS = true;
    SHOW_CHUNK_EXTENT = true;
    SHOW_INFO = true;
    SHOW_FOCUS = true;
    SHOW_VIEWBOX = true;
  }

  void SignalChange() { om::event::PreferenceChange(0); }

  void Load() { log_errors << "om::prefs::View3d::Load() not yet implemented"; }

  void Save() { log_errors << "om::prefs::View3d::Save() not yet implemented"; }
};
}
}  // om::prefs3d::
