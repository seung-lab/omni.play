#ifndef OM_DRAW_OPTIONS_H
#define OM_DRAW_OPTIONS_H

/*
 *	DrawOptions for use with a Bitfield.  Primarily used to set options for 
 *	the DrawCuller which then propogates through the draw tree.
 */


#define DRAWOP_RENDERMODE_MASK		   0x000F0000
#define DRAWOP_RENDERMODE_SELECTION	   0x00010000
#define DRAWOP_RENDERMODE_RENDER	   0x00020000


#define DRAWOP_DRAW_MASK		   0x000000F0
#define DRAWOP_DRAW_VOLUME_AXIS		   0x00000010
#define DRAWOP_DRAW_CHUNK_EXTENT	   0x00000020
#define DRAWOP_DRAW_WIDGETS		   0x00000040


#define DRAWOP_LEVEL_MASK		   0x0000000F
#define DRAWOP_LEVEL_SEGMENT		   0x00000001
#define DRAWOP_LEVEL_CHUNKS		   0x00000002
#define DRAWOP_LEVEL_VOLUME		   0x00000004
#define DRAWOP_LEVEL_ALL		   (DRAWOP_LEVEL_SEGMENT | DRAWOP_LEVEL_CHUNKS | DRAWOP_LEVEL_VOLUME)


#define DRAWOP_SEGMENT_FILTER_MASK	   0x00000F00
#define DRAWOP_SEGMENT_FILTER_SELECTED	   0x00000100
#define DRAWOP_SEGMENT_FILTER_UNSELECTED   0x00000200

#define DRAWOP_SEGMENT_COLOR_MASK	   0x0000F000
#define DRAWOP_SEGMENT_COLOR_TRANSPARENT   0x00001000
#define DRAWOP_SEGMENT_COLOR_HIGHLIGHT	   0x00002000



/*
 *	gl Name Constants
 */

#define OMGL_NAME_MESH	0
#define OMGL_NAME_VOXEL	1

#endif
