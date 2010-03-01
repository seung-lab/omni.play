#ifndef OM_PREFERENCES_DEFINITIONS_H
#define OM_PREFERENCES_DEFINITIONS_H

/*
 *
 *	Brett Warne - bwarne@mit.edu - 4/8/09
 */

//
//	Preference Naming Convetion:
//	Follow this to make debugging (and more generally, life) easier.
//
//	Prefix each preference with OM_PREF_
//	Then append primary library using the preference. ex) VIEW3D_
//	Then append name of preference. ex) BACKGROUND_COLOR_
//	Postfix with type: STR, FLT, INT, BOOL, V3F
//	
//	Keep sorted first by library, then alphabetically.


enum {
	//GUI
	OM_PREF_GUI_VIEWBOX_MOVE_RIGHT_STR = 100,			
	OM_PREF_GUI_VIEWBOX_MOVE_LEFT_STR,			
	OM_PREF_GUI_VIEWBOX_MOVE_UP_STR,				
	OM_PREF_GUI_VIEWBOX_MOVE_DOWN_STR,			

	OM_PREF_GUI_VIEWBOX_ZOOM_IN_STR,				
	OM_PREF_GUI_VIEWBOX_ZOOM_OUT_STR,			

	OM_PREF_GUI_VIEW_CENTER_FORWARD_STR,			
	OM_PREF_GUI_VIEW_CENTER_BACKWARD_STR,		

	OM_PREF_GUI_UNDO_STR,						
	OM_PREF_GUI_REDO_STR,						

	//MESH
	OM_PREF_MESH_NUM_SMOOTHING_ITERS_INT = 200,		
	OM_PREF_MESH_PRESERVED_SHARP_ANGLE_FLT,		
	OM_PREF_MESH_REDUCTION_PERCENT_FLT,	
	
	//VIEW2D
	OM_PREF_VIEW2D_DEPTH_CACHE_SIZE_INT = 500,
	OM_PREF_VIEW2D_MIP_CACHE_SIZE_INT,
	OM_PREF_VIEW2D_SHOW_INFO_BOOL,
	OM_PREF_VIEW2D_SIDES_CACHE_SIZE_INT,
	OM_PREF_VIEW2D_TILE_CACHE_SIZE_INT,
	OM_PREF_VIEW2D_TRANSPARENT_ALPHA_FLT,
	OM_PREF_VIEW2D_VOLUME_CACHE_SIZE_INT,

	//VIEW3D	
	OM_PREF_VIEW3D_ANTIALIASING_BOOL = 600,			
	OM_PREF_VIEW3D_BACKGROUND_COLOR_V3F,			
	OM_PREF_VIEW3D_CAMERA_FAR_PLANE_FLT,			
	OM_PREF_VIEW3D_CAMERA_FOV_FLT,				
	OM_PREF_VIEW3D_CAMERA_NEAR_PLANE_FLT,		
	OM_PREF_VIEW3D_FOCUS_STYLE_INT,				
	OM_PREF_VIEW3D_HIGHLIGHT_COLOR_V3F,			
	OM_PREF_VIEW3D_HIGHLIGHT_ENABLED_BOOL,		
	OM_PREF_VIEW3D_SHOW_AXIS_BOOL,				
	OM_PREF_VIEW3D_SHOW_CHUNK_EXTENT_BOOL,		
	OM_PREF_VIEW3D_SHOW_INFO_BOOL,				
	OM_PREF_VIEW3D_SHOW_FOCUS_BOOL,				
	OM_PREF_VIEW3D_SHOW_VIEWBOX_BOOL,			
	OM_PREF_VIEW3D_TRANSPARENT_ALPHA_FLT,		
	OM_PREF_VIEW3D_TRANSPARENT_UNSELECTED_BOOL
	
};

// functions to set default preferences
void omSetDefaultAllPreferences();
void omSetDefaultCommonPreferences();
void omSetDefaultGuiPreferences();
void omSetDefaultMeshPreferences();
void omSetDefaultSegmentPreferences();
void omSetDefaultView2dPreferences();
void omSetDefaultView3dPreferences();

#endif
