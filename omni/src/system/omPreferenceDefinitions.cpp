#include <stdio.h>
#include "omPreferenceDefinitions.h"
#include "omPreferences.h"
#include "common/omDebug.h"
#include "system/omNumCores.h"

#pragma mark
#pragma mark All Default Preferences
/////////////////////////////////
///////          All Default Preferences

void omSetDefaultAllPreferences()
{
	//set all library default prefs
	omSetDefaultCommonPreferences();
	omSetDefaultGuiPreferences();
	omSetDefaultMeshPreferences();
	omSetDefaultSegmentPreferences();
	omSetDefaultSystemPreferences();
	omSetDefaultView2dPreferences();
	omSetDefaultView3dPreferences();
}

#pragma mark
#pragma mark Individual Library Default Preferences
/////////////////////////////////
///////          Individual Library Default Preferences

void omSetDefaultCommonPreferences()
{

}

void omSetDefaultGuiPreferences()
{
	OmPreferences::Set(OM_PREF_GUI_VIEWBOX_MOVE_RIGHT_STR, "Right");
	OmPreferences::Set(OM_PREF_GUI_VIEWBOX_MOVE_LEFT_STR, "Left");
	OmPreferences::Set(OM_PREF_GUI_VIEWBOX_MOVE_UP_STR, "Up");
	OmPreferences::Set(OM_PREF_GUI_VIEWBOX_MOVE_DOWN_STR, "Down");

	OmPreferences::Set(OM_PREF_GUI_VIEWBOX_ZOOM_IN_STR, "Ctrl+Plus");
	OmPreferences::Set(OM_PREF_GUI_VIEWBOX_ZOOM_OUT_STR, "Ctrl+Minus");

	OmPreferences::Set(OM_PREF_GUI_VIEW_CENTER_FORWARD_STR, "Ctrl+]");
	OmPreferences::Set(OM_PREF_GUI_VIEW_CENTER_BACKWARD_STR, "Ctrl+[");

	OmPreferences::Set(OM_PREF_GUI_UNDO_STR, "Ctrl+Z");
	OmPreferences::Set(OM_PREF_GUI_REDO_STR, "Ctrl+Y");
}

void omSetDefaultMeshPreferences()
{

	OmPreferences::Set(OM_PREF_MESH_NUM_SMOOTHING_ITERS_INT, 50);
	OmPreferences::Set(OM_PREF_MESH_PRESERVED_SHARP_ANGLE_FLT, 30.0f);
	OmPreferences::Set(OM_PREF_MESH_REDUCTION_PERCENT_FLT, 0.15f);
			   
	const int numCoresRaw = (int)OmNumCores::get_num_cores();
	int numCores = numCoresRaw - 1;
	if( 1 == numCoresRaw ){
		numCores = 1;
	}
	OmPreferences::Set(OM_PREF_MESH_NUM_MESHING_THREADS_INT, numCores );
}

void omSetDefaultSegmentPreferences()
{
	OmPreferences::Set(OM_PREF_VIEW3D_HIGHLIGHT_COLOR_V3F, Vector3f::ONE);
}

void omSetDefaultSystemPreferences()
{
	OmPreferences::Set(OM_PREF_SYSTEM_RAM_GROUP_CACHE_MAX_MB_FLT, 256.0f);
	OmPreferences::Set(OM_PREF_SYSTEM_VRAM_GROUP_CACHE_MAX_MB_FLT, 128.0f);
}

void omSetDefaultView2dPreferences()
{
	OmPreferences::Set(OM_PREF_VIEW2D_VOLUME_CACHE_SIZE_INT, 500);
	OmPreferences::Set(OM_PREF_VIEW2D_TILE_CACHE_SIZE_INT, 300);
	OmPreferences::Set(OM_PREF_VIEW2D_DEPTH_CACHE_SIZE_INT, 20);
	OmPreferences::Set(OM_PREF_VIEW2D_SIDES_CACHE_SIZE_INT, 5);
	OmPreferences::Set(OM_PREF_VIEW2D_MIP_CACHE_SIZE_INT, 1);
	OmPreferences::Set(OM_PREF_VIEW2D_TRANSPARENT_ALPHA_FLT, 0.5f);
	OmPreferences::Set(OM_PREF_VIEW2D_SHOW_INFO_BOOL, true);
}

void omSetDefaultView3dPreferences()
{

	OmPreferences::Set(OM_PREF_VIEW3D_BACKGROUND_COLOR_V3F, Vector3f::ZERO);
	OmPreferences::Set(OM_PREF_VIEW3D_FOCUS_STYLE_INT, 1);
	OmPreferences::Set(OM_PREF_VIEW3D_HIGHLIGHT_COLOR_V3F, Vector3f::ONE);
	OmPreferences::Set(OM_PREF_VIEW3D_HIGHLIGHT_ENABLED_BOOL, true);
	OmPreferences::Set(OM_PREF_VIEW3D_SHOW_AXIS_BOOL, false);
	OmPreferences::Set(OM_PREF_VIEW3D_SHOW_CHUNK_EXTENT_BOOL, true);

	OmPreferences::Set(OM_PREF_VIEW3D_SHOW_INFO_BOOL, false);
	OmPreferences::Set(OM_PREF_VIEW3D_SHOW_FOCUS_BOOL, true);
	OmPreferences::Set(OM_PREF_VIEW3D_SHOW_VIEWBOX_BOOL, true);
	OmPreferences::Set(OM_PREF_VIEW3D_TRANSPARENT_ALPHA_FLT, 0.2f);
	OmPreferences::Set(OM_PREF_VIEW3D_TRANSPARENT_UNSELECTED_BOOL, false);

	OmPreferences::Set(OM_PREF_VIEW3D_ANTIALIASING_BOOL, false);
	OmPreferences::Set(OM_PREF_VIEW3D_CAMERA_FOV_FLT, 60.0f);
	OmPreferences::Set(OM_PREF_VIEW3D_CAMERA_NEAR_PLANE_FLT, 1.0f);
	OmPreferences::Set(OM_PREF_VIEW3D_CAMERA_FAR_PLANE_FLT, 1000.0f);
}
