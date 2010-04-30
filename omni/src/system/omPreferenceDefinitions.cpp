#include <stdio.h>
#include "omPreferenceDefinitions.h"
#include "omPreferences.h"
#include "common/omDebug.h"

/////////////////////////////////
///////          All Default Preferences

void omSetDefaultAllPreferences()
{
	omSetDefaultGuiPreferences();
	omSetDefaultMeshPreferences();
	omSetDefaultSegmentPreferences();
	omSetDefaultView2dPreferences();
	omSetDefaultView3dPreferences();
}

void omSetDefaultGuiPreferences()
{
	OmPreferences::SetString(OM_PREF_GUI_VIEWBOX_MOVE_RIGHT_STR, "Right");
	OmPreferences::SetString(OM_PREF_GUI_VIEWBOX_MOVE_LEFT_STR, "Left");
	OmPreferences::SetString(OM_PREF_GUI_VIEWBOX_MOVE_UP_STR, "Up");
	OmPreferences::SetString(OM_PREF_GUI_VIEWBOX_MOVE_DOWN_STR, "Down");

	OmPreferences::SetString(OM_PREF_GUI_VIEWBOX_ZOOM_IN_STR, "Ctrl+Plus");
	OmPreferences::SetString(OM_PREF_GUI_VIEWBOX_ZOOM_OUT_STR, "Ctrl+Minus");

	OmPreferences::SetString(OM_PREF_GUI_VIEW_CENTER_FORWARD_STR, "Ctrl+]");
	OmPreferences::SetString(OM_PREF_GUI_VIEW_CENTER_BACKWARD_STR, "Ctrl+[");

	OmPreferences::SetString(OM_PREF_GUI_UNDO_STR, "Ctrl+Z");
	OmPreferences::SetString(OM_PREF_GUI_REDO_STR, "Ctrl+Y");
}

void omSetDefaultMeshPreferences()
{
	OmPreferences::SetInteger(OM_PREF_MESH_NUM_SMOOTHING_ITERS_INT, 50);
	OmPreferences::SetFloat(OM_PREF_MESH_PRESERVED_SHARP_ANGLE_FLT, 30.0f);
	OmPreferences::SetFloat(OM_PREF_MESH_REDUCTION_PERCENT_FLT, 35.0f);
}

void omSetDefaultSegmentPreferences()
{
	OmPreferences::SetVector3f(OM_PREF_VIEW3D_HIGHLIGHT_COLOR_V3F, Vector3f::ONE);
}

void omSetDefaultView2dPreferences()
{
	OmPreferences::SetInteger(OM_PREF_VIEW2D_VOLUME_CACHE_SIZE_INT, 500);
	OmPreferences::SetInteger(OM_PREF_VIEW2D_TILE_CACHE_SIZE_INT, 300);
	OmPreferences::SetInteger(OM_PREF_VIEW2D_DEPTH_CACHE_SIZE_INT, 20);
	OmPreferences::SetInteger(OM_PREF_VIEW2D_SIDES_CACHE_SIZE_INT, 5);
	OmPreferences::SetInteger(OM_PREF_VIEW2D_MIP_CACHE_SIZE_INT, 1);
	OmPreferences::SetFloat(OM_PREF_VIEW2D_TRANSPARENT_ALPHA_FLT, 0.5f);
	OmPreferences::SetBoolean(OM_PREF_VIEW2D_SHOW_INFO_BOOL, true);
}

void omSetDefaultView3dPreferences()
{
	OmPreferences::SetVector3f(OM_PREF_VIEW3D_BACKGROUND_COLOR_V3F, Vector3f::ZERO);
	OmPreferences::SetInteger(OM_PREF_VIEW3D_FOCUS_STYLE_INT, 1);
	OmPreferences::SetVector3f(OM_PREF_VIEW3D_HIGHLIGHT_COLOR_V3F, Vector3f::ONE);
	OmPreferences::SetBoolean(OM_PREF_VIEW3D_HIGHLIGHT_ENABLED_BOOL, false);
	OmPreferences::SetBoolean(OM_PREF_VIEW3D_SHOW_AXIS_BOOL, true);
	OmPreferences::SetBoolean(OM_PREF_VIEW3D_SHOW_CHUNK_EXTENT_BOOL, true);

	OmPreferences::SetBoolean(OM_PREF_VIEW3D_SHOW_INFO_BOOL, true);
	OmPreferences::SetBoolean(OM_PREF_VIEW3D_SHOW_FOCUS_BOOL, true);
	OmPreferences::SetBoolean(OM_PREF_VIEW3D_SHOW_VIEWBOX_BOOL, true);
	OmPreferences::SetFloat(OM_PREF_VIEW3D_TRANSPARENT_ALPHA_FLT, 0.2f);
	OmPreferences::SetBoolean(OM_PREF_VIEW3D_TRANSPARENT_UNSELECTED_BOOL, false);

	OmPreferences::SetBoolean(OM_PREF_VIEW3D_ANTIALIASING_BOOL, false);
	OmPreferences::SetFloat(OM_PREF_VIEW3D_CAMERA_FOV_FLT, 60.0f);
	OmPreferences::SetFloat(OM_PREF_VIEW3D_CAMERA_NEAR_PLANE_FLT, 1.0f);
	OmPreferences::SetFloat(OM_PREF_VIEW3D_CAMERA_FAR_PLANE_FLT, 1000000.0f);
}
