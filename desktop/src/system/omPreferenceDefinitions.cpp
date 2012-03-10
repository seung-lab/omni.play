#include <stdio.h>
#include "omPreferenceDefinitions.h"
#include "omPreferences.h"

#define SET_STR OmPreferences::SetString
#define SET_INT OmPreferences::SetInteger
#define SET_FLOAT OmPreferences::SetFloat
#define SET_BOOL OmPreferences::SetBoolean
#define SET_V3F OmPreferences::SetVector3f

void OmPreferenceDefaults::setDefaultView2dPreferences()
{
    SET_INT(om::PREF_VIEW2D_VOLUME_CACHE_SIZE_INT, 500);
    SET_INT(om::PREF_VIEW2D_TILE_CACHE_SIZE_INT, 300);
    SET_INT(om::PREF_VIEW2D_DEPTH_CACHE_SIZE_INT, 20);
    SET_INT(om::PREF_VIEW2D_SIDES_CACHE_SIZE_INT, 5);
    SET_INT(om::PREF_VIEW2D_MIP_CACHE_SIZE_INT, 1);
    SET_FLOAT(om::PREF_VIEW2D_TRANSPARENT_ALPHA_FLT, 0.5f);
    SET_BOOL(om::PREF_VIEW2D_SHOW_INFO_BOOL, true);
}

void OmPreferenceDefaults::setDefaultView3dPreferences()
{
    SET_V3F(om::PREF_VIEW3D_BACKGROUND_COLOR_V3F, Vector3f::ZERO);
    SET_INT(om::PREF_VIEW3D_FOCUS_STYLE_INT, 1);
    SET_V3F(om::PREF_VIEW3D_HIGHLIGHT_COLOR_V3F, Vector3f::ONE);
    SET_BOOL(om::PREF_VIEW3D_HIGHLIGHT_ENABLED_BOOL, false);
    SET_BOOL(om::PREF_VIEW3D_SHOW_AXIS_BOOL, true);
    SET_BOOL(om::PREF_VIEW3D_SHOW_CHUNK_EXTENT_BOOL, true);

    SET_BOOL(om::PREF_VIEW3D_SHOW_INFO_BOOL, true);
    SET_BOOL(om::PREF_VIEW3D_SHOW_FOCUS_BOOL, true);
    SET_BOOL(om::PREF_VIEW3D_SHOW_VIEWBOX_BOOL, true);
    SET_FLOAT(om::PREF_VIEW3D_TRANSPARENT_ALPHA_FLT, 0.2f);
    SET_BOOL(om::PREF_VIEW3D_TRANSPARENT_UNSELECTED_BOOL, false);

    SET_BOOL(om::PREF_VIEW3D_ANTIALIASING_BOOL, false);
    SET_FLOAT(om::PREF_VIEW3D_CAMERA_FOV_FLT, 60.0f);
    SET_FLOAT(om::PREF_VIEW3D_CAMERA_NEAR_PLANE_FLT, 1.0f);
    SET_FLOAT(om::PREF_VIEW3D_CAMERA_FAR_PLANE_FLT, 1000000.0f);
}

#undef SET_STR
#undef SET_INT
#undef SET_FLOAT
#undef SET_BOOL
#undef SET_V3F
