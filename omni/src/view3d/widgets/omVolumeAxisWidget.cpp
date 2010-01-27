
#include "omSelectionWidget.h"

#include "view3d/omCamera.h"
#include "view3d/omView3d.h"

#include "volume/omMipChunkCuller.h"
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

 OmSelectionWidget::OmSelectionWidget(OmView3d * view3d):OmView3dWidget(view3d)
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
void OmSelectionWidget::Draw()
{

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	//clear and setup stencil
	glClearStencil(0);
	glClear(GL_STENCIL_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);

	//disable lighting (faster draw)
	glDisable(GL_LIGHTING);
	//selected visiable over everything
	glDisable(GL_DEPTH_TEST);

	//set stencil buff to write 1 every for every pixel written
	glStencilFunc(GL_ALWAYS, 1, 0xFFFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	//draw with fill
	glPolygonMode(GL_FRONT, GL_FILL);

	//offset since this stencil filling draw is on top of real draw
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(-2.5f, -2.5f);	//closer to camera

	//disable writing to color and depth buffer
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);

	//draw to paint stencil buffer
	mView3d->DrawVolumes(CULLER_LEVEL_ALL | CULLER_FILTER_SELECTED | CULLER_HIGHLIGHT_SEGMENTS);

	//stencil buff only allows writing to screen when buff not 1
	glStencilFunc(GL_NOTEQUAL, 1, 0xFFFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	//set to line mode
	glPolygonMode(GL_FRONT, GL_LINE);
	glLineWidth(5.0f);

	//enable writing to color buffer
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	//glDepthMask(GL_TRUE);
	mView3d->DrawVolumes(CULLER_LEVEL_ALL | CULLER_FILTER_SELECTED | CULLER_HIGHLIGHT_SEGMENTS);

	glPopAttrib();
}
