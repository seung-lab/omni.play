#include "common/omCommon.h"
#include "common/omDebug.h"
#include "common/omGl.h"
#include "omViewBoxWidget.h"
#include "project/omProject.h"
#include "system/omLocalPreferences.h"
#include "system/omStateManager.h"
#include "view2d/drawable.h"
#include "volume/omChannel.h"

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
	//glDepthMask(GL_FALSE);

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
	if(!OmLocalPreferences::get2DViewPaneIn3D())
	{
		return;
	}
	if(!OmProject::IsChannelValid(1)) {
		return;
	}
	OmChannel& channel = OmProject::GetChannel(1);
	Vector3f resolution = channel.GetDataResolution();
	Vector3i extents = channel.GetDataDimensions();

	glColor3fv(OMGL_WHITE);
        glEnable(GL_TEXTURE_2D);
	glDepthMask(GL_TRUE);
	//glEnable(GL_BLEND);
	//glDisable(GL_DEPTH_TEST);
        //glBlendFunc(GL_ONE, GL_DST_ALPHA);	

	Vector2f dataMin,dataMax,spaceMin,spaceMax;

	for (vector < Drawable * >::iterator it = drawables.begin(); drawables.end() != it; it++) {
		Drawable *d = *it;

		int level = d->tileCoord.Level;
		Vector3f tileLength = resolution*128.0*OMPOW(2,level);

		SpaceCoord thisCoord = d->tileCoord.Coordinate;
		debug ("FIXME", "thisCoord.(x,y,z): (%f,%f,%f)\n", DEBUGV3(thisCoord));
		NormCoord normCoord = channel.SpaceToNormCoord(d->tileCoord.Coordinate);
		//thisCoord = channel.NormToDataCoord(normCoord);

		debug ("FIXME", "normCoord.(x,y,z): (%f,%f,%f)\n", DEBUGV3(normCoord));
		glBindTexture(GL_TEXTURE_2D, d->gotten_id->GetTextureID());
		glBegin(GL_QUADS);

	if (plane == XY_VIEW) {
		if (GetTextureMax(thisCoord, plane, dataMax, spaceMax)){
			spaceMax.x = thisCoord.x + tileLength.x;
			spaceMax.y = thisCoord.y + tileLength.y;
			dataMax.x = 1.0;
			dataMax.y = 1.0;
		}
		if (GetTextureMin(thisCoord, plane, dataMin, spaceMin)){
			spaceMin.x = thisCoord.x;
			spaceMin.y = thisCoord.y;
			dataMin.x = 0.0;
			dataMin.y = 0.0;
		}
		glTexCoord2f(dataMin.x, dataMin.y);	/* lower left corner of image */
		glVertex3f(spaceMin.x, spaceMin.y,thisCoord.z);

		glTexCoord2f(dataMax.x, dataMin.y);	/* lower right corner of image */
		glVertex3f(spaceMax.x, spaceMin.y,thisCoord.z);

		glTexCoord2f(dataMax.x, dataMax.y);	/* upper right corner of image */
		glVertex3f(spaceMax.x, spaceMax.y, thisCoord.z);

		glTexCoord2f(dataMin.x, dataMax.y);	/* upper left corner of image */
		glVertex3f(spaceMin.x, spaceMax.y, thisCoord.z);
		glEnd();
	} else if (plane == XZ_VIEW) {
		if (GetTextureMax(thisCoord, plane, dataMax, spaceMax)){
			spaceMax.x = thisCoord.x + tileLength.x;
			spaceMax.y = thisCoord.z + tileLength.z;
			dataMax.x = 1.0;
			dataMax.y = 1.0;
		}
		if (GetTextureMin(thisCoord, plane, dataMin, spaceMin)){
			spaceMin.x = thisCoord.x;
			spaceMin.y = thisCoord.z;
			dataMin.x = 0.0;
			dataMin.y = 0.0;
		}
		glTexCoord2f(dataMin.x, dataMin.y);	/* lower left corner of image */
		glVertex3f(spaceMin.x, thisCoord.y,spaceMin.y);

		glTexCoord2f(dataMax.x, dataMin.y);	/* lower right corner of image */
		glVertex3f(spaceMax.x, thisCoord.y,spaceMin.y);

		glTexCoord2f(dataMax.x, dataMax.y);	/* upper right corner of image */
		glVertex3f(spaceMax.x, thisCoord.y,spaceMax.y);

		glTexCoord2f(dataMin.x, dataMax.y);	/* upper left corner of image */
		glVertex3f(spaceMin.x, thisCoord.y,spaceMax.y);
		glEnd();
	} else if (plane == YZ_VIEW) {
		if (GetTextureMax(thisCoord, plane, dataMax, spaceMax)){
			spaceMax.x = thisCoord.z + tileLength.z;
			spaceMax.y = thisCoord.y + tileLength.y;
			dataMax.x = 1.0;
			dataMax.y = 1.0;
		}
		if (GetTextureMin(thisCoord, plane, dataMin, spaceMin)){
			spaceMin.x = thisCoord.z;
			spaceMin.y = thisCoord.y;
			dataMin.x = 0.0;
			dataMin.y = 0.0;
		}

		debug ("chandata", "dataMin.(x,y): (%f,%f)\n", dataMin.x,dataMin.y);
		debug ("chandata", "dataMax.(x,y): (%f,%f)\n", dataMax.x,dataMax.y);
		debug ("chandata", "spaceMin.(x,y): (%f,%f)\n", spaceMin.x,spaceMin.y);
		debug ("chandata", "spaceMax.(x,y): (%f,%f)\n", spaceMax.x,spaceMax.y);
		glTexCoord2f(dataMin.x, dataMin.y);	/* lower left corner of image */
		glVertex3f(thisCoord.x,spaceMin.x,spaceMin.y);

		glTexCoord2f(dataMax.x, dataMin.y);	/* lower right corner of image */
		glVertex3f(thisCoord.x,spaceMax.x ,spaceMin.y);

		glTexCoord2f(dataMax.x, dataMax.y);	/* upper right corner of image */
		glVertex3f(thisCoord.x,spaceMax.x,spaceMax.y);

		glTexCoord2f(dataMin.x, dataMax.y);	/* upper left corner of image */
		glVertex3f(thisCoord.x,spaceMin.x,spaceMax.y);
		glEnd();
	}

	}
	//glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}

bool 
OmViewBoxWidget::GetTextureMax(Vector3f coord,ViewType plane, Vector2f & dataMax, Vector2f & spaceMax)
{
	OmChannel& channel = OmProject::GetChannel( 1);
	Vector3f resolution = channel.GetDataResolution();
	Vector3f tileLength = resolution*128.0;
	Vector2f maxScreen = mViewGroupState->GetViewSliceMax(plane);
	DataCoord maxData = channel.GetDataExtent().getMax();
	NormCoord maxNorm = channel.DataToNormCoord(maxData);
	SpaceCoord maxSpace= channel.NormToSpaceCoord(maxNorm);
	SpaceCoord maxLimit = maxSpace.compareMinimum(coord+tileLength);
	bool result;

	switch (plane) {
	case XY_VIEW:
		if (maxLimit.x>maxScreen.x) spaceMax.x=maxScreen.x; else spaceMax.x=maxLimit.x;
		if (maxLimit.y>maxScreen.y) spaceMax.y=maxScreen.y; else spaceMax.y=maxLimit.y;
		dataMax.x = (spaceMax.x-coord.x)/tileLength.x;
		dataMax.y = (spaceMax.y-coord.y)/tileLength.y;
		result = ((coord.x + tileLength.x) < spaceMax.x);
		result = result && ((coord.y + tileLength.y) < spaceMax.y);
		break;
	case XZ_VIEW:
		if (maxLimit.x>maxScreen.x) spaceMax.x=maxScreen.x; else spaceMax.x=maxLimit.x;
		if (maxLimit.z>maxScreen.y) spaceMax.y=maxScreen.y; else spaceMax.y=maxLimit.z;
		dataMax.x = (spaceMax.x-coord.x)/tileLength.x;
		dataMax.y = (spaceMax.y-coord.z)/tileLength.z;
		result = ((coord.x + tileLength.x) < spaceMax.x);
		result = result && ((coord.z + tileLength.z) < spaceMax.y);
		break;
	case YZ_VIEW:
		if (maxLimit.z>maxScreen.y) spaceMax.y=maxScreen.y; else spaceMax.y=maxLimit.z;
		if (maxLimit.y>maxScreen.x) spaceMax.x=maxScreen.x; else spaceMax.x=maxLimit.y;
		dataMax.x = (spaceMax.x-coord.y)/tileLength.y;
		dataMax.y = (spaceMax.y-coord.z)/tileLength.z;
		result = ((coord.y + tileLength.y) < spaceMax.x);
		result = result && ((coord.z + tileLength.z) < spaceMax.y);
		break;
	default:
		assert(0);
	}
	return result;
}

bool 
OmViewBoxWidget::GetTextureMin(Vector3f coord,ViewType plane, Vector2f & dataMin, Vector2f & spaceMin)
{

	OmChannel& channel = OmProject::GetChannel( 1);
	Vector3f resolution = channel.GetDataResolution();
	Vector3f tileLength = resolution*128.0;
        Vector2f minScreen = mViewGroupState->GetViewSliceMin(plane);
	DataCoord minData = channel.GetDataExtent().getMin();
	NormCoord minNorm = channel.DataToNormCoord(minData);
	SpaceCoord minSpace= channel.NormToSpaceCoord(minNorm);
	SpaceCoord minLimit = minSpace.compareMaximum(coord);
	bool result;	

	switch (plane) {
	case XY_VIEW:
		if (minLimit.x<minScreen.x) spaceMin.x=minScreen.x; else spaceMin.x=minLimit.x;
		if (minLimit.y<minScreen.y) spaceMin.y=minScreen.y; else spaceMin.y=minLimit.y;
		dataMin.x = (spaceMin.x - coord.x)/tileLength.x;
		dataMin.y = (spaceMin.y - coord.y)/tileLength.y;
		result = (coord.x > spaceMin.x);
		result = result && (coord.y > spaceMin.y);
		break;
	case XZ_VIEW:
		if (minLimit.x<minScreen.x) spaceMin.x=minScreen.x; else spaceMin.x=minLimit.x;
		if (minLimit.z<minScreen.y) spaceMin.y=minScreen.y; else spaceMin.y=minLimit.z;
		dataMin.x = (spaceMin.x - coord.x)/tileLength.x;
		dataMin.y = (spaceMin.y - coord.z)/tileLength.z;
		result = (coord.x > spaceMin.x);
		result = result && (coord.z > spaceMin.y);
		break;
	case YZ_VIEW:
		if (minLimit.z<minScreen.y) spaceMin.y=minScreen.y; else spaceMin.y=minLimit.z;
		if (minLimit.y<minScreen.x) spaceMin.x=minScreen.x; else spaceMin.x=minLimit.y;
		dataMin.x = (spaceMin.x - coord.y)/tileLength.y;
		dataMin.y = (spaceMin.y - coord.z)/tileLength.z;
		result = (coord.z > spaceMin.y);
		result = result && (coord.y > spaceMin.x);
		break;
	default:
		assert(0);
	}
	return result;
}
