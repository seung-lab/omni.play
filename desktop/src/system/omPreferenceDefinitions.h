#pragma once

/**
 *Preference Naming Convetion:
 *
 *  Prefix each preference with om::PREF_
 *  Then append primary library using the preference. ex) VIEW3D_
 *  Then append name of preference. ex) BACKGROUND_COLOR_
 *  Postfix with type: STR, FLT, INT, BOOL, V3F
 *
 *  Keep sorted first by library, then alphabetically.
 *
 *  Brett Warne - bwarne@mit.edu - 4/8/09
 */

namespace om {
enum PrefEnum {

  //VIEW2D
  PREF_VIEW2D_DEPTH_CACHE_SIZE_INT = 500,
  PREF_VIEW2D_MIP_CACHE_SIZE_INT = 501,
  PREF_VIEW2D_SHOW_INFO_BOOL = 502,
  PREF_VIEW2D_SIDES_CACHE_SIZE_INT = 503,
  PREF_VIEW2D_TILE_CACHE_SIZE_INT = 504,
  PREF_VIEW2D_TRANSPARENT_ALPHA_FLT = 505,
  PREF_VIEW2D_VOLUME_CACHE_SIZE_INT = 506,

  //VIEW3D
  PREF_VIEW3D_ANTIALIASING_BOOL = 600,
  PREF_VIEW3D_BACKGROUND_COLOR_V3F = 601,
  PREF_VIEW3D_CAMERA_FAR_PLANE_FLT = 602,
  PREF_VIEW3D_CAMERA_FOV_FLT = 603,
  PREF_VIEW3D_CAMERA_NEAR_PLANE_FLT = 604,
  PREF_VIEW3D_FOCUS_STYLE_INT = 605,
  PREF_VIEW3D_HIGHLIGHT_COLOR_V3F = 606,
  PREF_VIEW3D_HIGHLIGHT_ENABLED_BOOL = 607,
  PREF_VIEW3D_SHOW_AXIS_BOOL = 608,
  PREF_VIEW3D_SHOW_CHUNK_EXTENT_BOOL = 609,
  PREF_VIEW3D_SHOW_INFO_BOOL = 610,
  PREF_VIEW3D_SHOW_FOCUS_BOOL = 611,
  PREF_VIEW3D_SHOW_VIEWBOX_BOOL = 612,
  PREF_VIEW3D_TRANSPARENT_ALPHA_FLT = 613,
  PREF_VIEW3D_TRANSPARENT_UNSELECTED_BOOL = 614
};

}  //namespace om

class OmPreferenceDefaults {
 public:
  static void SetDefaultAllPreferences() {
    setDefaultView2dPreferences();
    setDefaultView3dPreferences();
  }

 private:
  static void setDefaultView2dPreferences();
  static void setDefaultView3dPreferences();
};
