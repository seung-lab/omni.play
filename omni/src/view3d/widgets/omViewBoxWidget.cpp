#include "omViewBoxWidget.h"

#include "system/omStateManager.h"
#include "common/omGl.h"
#include "system/omLocalPreferences.h"

enum OmViewBoxPlane { XY_PLANE, XZ_PLANE, YZ_PLANE };
static const int RECT_WIREFRAME_LINE_WIDTH = 2;

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
		drawSlice(XY_VIEW, 
			  OmStateManager::GetViewSliceMin(XY_VIEW), 
			  OmStateManager::GetViewSliceMax(XY_VIEW),
			  OmStateManager::GetViewSliceDepth(XY_VIEW));

		drawSlice(XZ_VIEW, 
			  OmStateManager::GetViewSliceMin(XZ_VIEW), 
			  OmStateManager::GetViewSliceMax(XZ_VIEW),
			  OmStateManager::GetViewSliceDepth(XZ_VIEW));

		drawSlice(YZ_VIEW, 
			  OmStateManager::GetViewSliceMin(YZ_VIEW), 
			  OmStateManager::GetViewSliceMax(YZ_VIEW),
			  OmStateManager::GetViewSliceDepth(YZ_VIEW));
	}
	
	if (OmLocalPreferences::getDrawCrosshairsIn3D()){
		drawLines(OmStateManager::GetViewDepthCoord());
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

