#include "omViewBoxWidget.h"

#include "system/omStateManager.h"
#include "common/omGl.h"
#include "system/omLocalPreferences.h"
#include "project/omProject.h"
#include "common/omCommon.h"
#include "volume/omChannel.h"
#include "view2d/drawable.h"

enum OmViewBoxPlane { XY_PLANE, XZ_PLANE, YZ_PLANE };
static const int RECT_WIREFRAME_LINE_WIDTH = 2;

OmViewBoxWidget::OmViewBoxWidget(OmView3d *view3d, OmViewGroupState * vgs) 
	: OmView3dWidget(view3d), mViewGroupState(vgs)
{ 
};

/**
 *	Draw the three orthogonal slices from of the view box
 */
void OmViewBoxWidget::Draw()
{
	//push attrs
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	//disable backface culling
	//glDisable(GL_CULL_FACE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//disable depth buffer
	glDepthMask(GL_FALSE);

	//disable lighting
	glDisable(GL_LIGHTING);

	//set line width
	glLineWidth(RECT_WIREFRAME_LINE_WIDTH);


	if (OmLocalPreferences::get2DViewFrameIn3D()){
		drawChannelData(XY_VIEW, OmStateManager::GetViewDrawable(XY_VIEW));
		drawSlice(XY_VIEW, 
			  mViewGroupState->GetViewSliceMin(XY_VIEW), 
			  mViewGroupState->GetViewSliceMax(XY_VIEW),
			  mViewGroupState->GetViewSliceDepth(XY_VIEW));
		drawChannelData(XZ_VIEW, OmStateManager::GetViewDrawable(XZ_VIEW));
		drawSlice(XZ_VIEW, 
			  mViewGroupState->GetViewSliceMin(XZ_VIEW), 
			  mViewGroupState->GetViewSliceMax(XZ_VIEW),
			  mViewGroupState->GetViewSliceDepth(XZ_VIEW));
		drawChannelData(YZ_VIEW, OmStateManager::GetViewDrawable(YZ_VIEW));
		drawSlice(YZ_VIEW, 
			  mViewGroupState->GetViewSliceMin(YZ_VIEW), 
			  mViewGroupState->GetViewSliceMax(YZ_VIEW),
			  mViewGroupState->GetViewSliceDepth(YZ_VIEW));
	}
	



	if (OmLocalPreferences::getDrawCrosshairsIn3D()){
		drawLines(mViewGroupState->GetViewDepthCoord());
	}

	glPopAttrib();
}

/**
 *	Draw a rectangle given the verticies in counter-clockwise order
 */
void OmViewBoxWidget::drawRectangle(SpaceCoord v0, SpaceCoord v1, SpaceCoord v2, SpaceCoord v3)
{
	glBegin(GL_LINE_STRIP);
	glVertex3fv(v0.array);
	glVertex3fv(v1.array);
	glVertex3fv(v2.array);
	glVertex3fv(v3.array);
	glVertex3fv(v0.array);
	glEnd();
}

void OmViewBoxWidget::drawLines(SpaceCoord depth)
{
	SpaceCoord v0, v1;

	float distance = ((float)OmLocalPreferences::getCrosshairValue())/10.0;

	glColor3fv(OMGL_BLUE);
	v0 = SpaceCoord(depth.x, depth.y, depth.z-distance);
	v1 = SpaceCoord(depth.x, depth.y, depth.z+distance);
	glBegin(GL_LINE_STRIP);
	glVertex3fv(v0.array);
	glVertex3fv(v1.array);	
	glEnd();

	glColor3fv(OMGL_GREEN);
	v0 = SpaceCoord(depth.x, depth.y-distance, depth.z);
	v1 = SpaceCoord(depth.x, depth.y+distance, depth.z);
	glBegin(GL_LINE_STRIP);
	glVertex3fv(v0.array);
	glVertex3fv(v1.array);	
	glEnd();

	glColor3fv(OMGL_RED);
	v0 = SpaceCoord(depth.x-distance, depth.y, depth.z);
	v1 = SpaceCoord(depth.x+distance, depth.y, depth.z);
	glBegin(GL_LINE_STRIP);
	glVertex3fv(v0.array);
	glVertex3fv(v1.array);	
	glEnd();
}

/**
 *	Draw a given orthogonal slice of a bbox given the plane and offset of plane
 */
void OmViewBoxWidget::drawSlice(ViewType plane, Vector2 < float >min, Vector2 < float >max, float depth)
{

	SpaceCoord v0, v1, v2, v3;

	switch (plane) {
	case XY_VIEW:
		glColor3fv(OMGL_BLUE);
		v0 = SpaceCoord(min.x, min.y, depth);
		v1 = SpaceCoord(max.x, min.y, depth);
		v2 = SpaceCoord(max.x, max.y, depth);
		v3 = SpaceCoord(min.x, max.y, depth);
		break;

	case XZ_VIEW:
		glColor3fv(OMGL_GREEN);
		v0 = SpaceCoord(min.x, depth, min.y);
		v1 = SpaceCoord(min.x, depth, max.y);
		v2 = SpaceCoord(max.x, depth, max.y);
		v3 = SpaceCoord(max.x, depth, min.y);
		break;

	case YZ_VIEW:
		glColor3fv(OMGL_RED);
		v0 = SpaceCoord(depth, min.x, min.y);
		v1 = SpaceCoord(depth, max.x, min.y);
		v2 = SpaceCoord(depth, max.x, max.y);
		v3 = SpaceCoord(depth, min.x, max.y);
		break;
	}

	drawRectangle(v0, v1, v2, v3);
}

void OmViewBoxWidget::drawChannelData(ViewType plane, vector<Drawable*> drawables)
{
	glColor3fv(OMGL_WHITE);
        glEnable(GL_TEXTURE_2D);
	glDepthMask(GL_TRUE);
	glEnable(GL_BLEND);
	//glDisable(GL_DEPTH_TEST);
        glBlendFunc(GL_ONE, GL_DST_ALPHA);	
	OmChannel& channel = OmProject::GetChannel( 1);

	Vector2f stretch = channel.GetStretchValues(plane);

	for (vector < Drawable * >::iterator it = drawables.begin(); drawables.end() != it; it++) {
		Drawable *d = *it;

		SpaceCoord thisCoord = d->tileCoord.Coordinate;
		Vector3f tileLength = channel.GetDataResolution()*128.0;
		/*if (mViewType == YZ_VIEW) {
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glTranslatef(0.5, 0.5, 0.0);
		glRotatef(-90, 0.0, 0.0, 1.0);
		glTranslatef(-0.5, -0.5, 0.0);
		glMatrixMode(GL_MODELVIEW);
		}*/
	debug ("chandata", "thisCoord.(x,y,z): (%f,%f,%f)\n", thisCoord.x,thisCoord.y,thisCoord.z);
	glBindTexture(GL_TEXTURE_2D, d->gotten_id->GetTextureID());
	glBegin(GL_QUADS);

	if (plane == XY_VIEW) {
		glTexCoord2f(0.0f, 0.0f);	/* lower left corner of image */
		glVertex3f(thisCoord.x, thisCoord.y,thisCoord.z);

		glTexCoord2f(1.0f, 0.0f);	/* lower right corner of image */
		glVertex3f((thisCoord.x + tileLength.x),thisCoord.y,thisCoord.z);

		glTexCoord2f(1.0f, 1.0f);	/* upper right corner of image */
		glVertex3f((thisCoord.x + tileLength.x), (thisCoord.y + tileLength.y),thisCoord.z);

		glTexCoord2f(0.0f, 1.0f);	/* upper left corner of image */
		glVertex3f(thisCoord.x, thisCoord.y + tileLength.y,thisCoord.z);
		glEnd();
	} else if (plane == XZ_VIEW) {
		glTexCoord2f(0.0f, 0.0f);	/* lower left corner of image */
		glVertex3f(thisCoord.x, thisCoord.y,thisCoord.z);

		glTexCoord2f(1.0f, 0.0f);	/* lower right corner of image */
		glVertex3f(thisCoord.x+tileLength.x, thisCoord.y,thisCoord.z);

		glTexCoord2f(1.0f, 1.0f);	/* upper right corner of image */
		glVertex3f(thisCoord.x+tileLength.x, thisCoord.y,thisCoord.z+tileLength.z);

		glTexCoord2f(0.0f, 1.0f);	/* upper left corner of image */
		glVertex3f(thisCoord.x, thisCoord.y,thisCoord.z+tileLength.z);
		glEnd();
	} else if (plane == YZ_VIEW) {
		glTexCoord2f(0.0f, 0.0f);	/* lower left corner of image */
		glVertex3f(thisCoord.x, thisCoord.y,thisCoord.z);

		glTexCoord2f(1.0f, 0.0f);	/* lower right corner of image */
		glVertex3f(thisCoord.x, thisCoord.y +tileLength.y,thisCoord.z);

		glTexCoord2f(1.0f, 1.0f);	/* upper right corner of image */
		glVertex3f(thisCoord.x, thisCoord.y+ tileLength.y,thisCoord.z+tileLength.z);

		glTexCoord2f(0.0f, 1.0f);	/* upper left corner of image */
		glVertex3f(thisCoord.x, thisCoord.y,thisCoord.z+tileLength.z);
		glEnd();
	}

	}
	//glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}
