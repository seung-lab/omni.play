
#include "omChunkExtentWidget.h"

#include "view3d/omCamera.h"
#include "view3d/omView3d.h"

#include "volume/omVolumeCuller.h"
#include "common/omGl.h"

#define DEBUG 0

static const float TEXT_COLOR[4] = { 1, 1, 1, 0.8 };

////////////////////////////////////////
///////         Utility Functions Prototypes

#pragma mark -
#pragma mark Example Class
/////////////////////////////////
///////
///////          Example Class
///////

OmChunkExtentWidget::OmChunkExtentWidget(OmView3d * view3d)
 : OmView3dWidget(view3d)
{

};

#pragma mark
#pragma mark Draw Methods
/////////////////////////////////
///////          Draw Methods

/*
 *	2-Pass Polygon Outlining using the Stencil Buffer
 *	http://www.codeproject.com/KB/openGL/Outline_Mode.aspx
 */
void
 OmChunkExtentWidget::Draw()
{

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	mView3d->DrawVolumes(DRAWOP_LEVEL_VOLUME | DRAWOP_LEVEL_CHUNKS | DRAWOP_DRAW_CHUNK_EXTENT);

	glPopAttrib();
}
