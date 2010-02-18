#include "omStateManager.h"
#include "omEventManager.h"
#include "project/omProject.h"
#include "events/omViewEvent.h"
#include "events/omView3dEvent.h"
#include "events/omSystemModeEvent.h"
#include "events/omToolModeEvent.h"

//undostack
#include <QUndoStack>

//view3d context
#include <QtOpenGL/qgl.h>
#include <QtOpenGL/QGLFormat>
#include "common/omDebug.h"

#define DEBUG 0

//init instance pointer
OmStateManager *OmStateManager::mspInstance = 0;

#pragma mark -
#pragma mark OmStateManager
/////////////////////////////////
///////
///////          OmStateManager
///////

OmStateManager::OmStateManager()
{

	//view3d
	mpPrimaryView3dWidget = NULL;
	mpUndoStack = NULL;

	mSystemMode = NAVIGATION_SYSTEM_MODE;
	mToolMode = ZOOM_MODE;

	//view slice data
	mViewSliceDataXY = NULL;
	mViewSliceDataYZ = NULL;
	mViewSliceDataXZ = NULL;

	mXYSliceEnabled = false;
	mYZSliceEnabled = false;
	mXZSliceEnabled = false;
}

OmStateManager::~OmStateManager()
{

	//undostack
	if (mpUndoStack) {
		delete mpUndoStack;
		mpUndoStack = NULL;
	}
	//view3d
	if (mpPrimaryView3dWidget) {
		delete mpPrimaryView3dWidget;
		mpPrimaryView3dWidget = NULL;
	}

}

/*
 * Static accessor
 */

OmStateManager *OmStateManager::Instance()
{
	if (NULL == mspInstance) {
		mspInstance = new OmStateManager;
	}
	return mspInstance;
}

/*
 *	Static destructor
 */
void OmStateManager::Delete()
{

	//delete self
	if (mspInstance) {
		delete mspInstance;
		mspInstance = NULL;
	}
}

#pragma mark
#pragma mark Project
/////////////////////////////////
///////          Project

//project
const string & OmStateManager::GetProjectFileName()
{
	return Instance()->mProjectFileName;
}

void OmStateManager::SetProjectFileName(const string & name)
{
	Instance()->mProjectFileName = name;
}

const string & OmStateManager::GetProjectDirectoryPath()
{
	return Instance()->mProjectDirectoryPath;
}

void OmStateManager::SetProjectDirectoryPath(const string & dpath)
{
	Instance()->mProjectDirectoryPath = dpath;

}

#pragma mark
#pragma mark Project State
/////////////////////////////////
///////          Project State

/*
void
OmStateManager::FlushState() {
	//commit project
	OmProject::Save();
	//clean undo stack
	Instance()->mpUndoStack->clear();
}
*/

#pragma mark
#pragma mark View Event
/////////////////////////////////
///////          View Event

/*
 *	Set/Get minimum coordiante of view slice.
 */
void OmStateManager::SetViewSliceMin(ViewType plane, Vector2 < float >point, bool postEvent)
{
	switch (plane) {
	case XY_VIEW:
		Instance()->mXYSlice[0] = point.x;
		Instance()->mXYSlice[1] = point.y;
		break;
	case XZ_VIEW:
		Instance()->mXZSlice[0] = point.x;
		Instance()->mXZSlice[1] = point.y;
		break;
	case YZ_VIEW:
		Instance()->mYZSlice[0] = point.x;
		Instance()->mYZSlice[1] = point.y;
		break;
	default:
		assert(false);
	}

	if (postEvent)
		OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_BOX_CHANGE));
}

Vector2 < float > OmStateManager::GetViewSliceMin(ViewType plane)
{
	switch (plane) {
	case XY_VIEW:
		return Vector2 < float >(&Instance()->mXYSlice[0]);
	case XZ_VIEW:
		return Vector2 < float >(&Instance()->mXZSlice[0]);
	case YZ_VIEW:
		return Vector2 < float >(&Instance()->mYZSlice[0]);
	default:
		assert(false);
	}
}

/*
 *	Set/Get maximum coordiante of view slice.
 */
void OmStateManager::SetViewSliceMax(ViewType plane, Vector2 < float >point, bool postEvent)
{
	switch (plane) {
	case XY_VIEW:
		Instance()->mXYSlice[2] = point.x;
		Instance()->mXYSlice[3] = point.y;
		break;
	case XZ_VIEW:
		Instance()->mXZSlice[2] = point.x;
		Instance()->mXZSlice[3] = point.y;
		break;
	case YZ_VIEW:
		Instance()->mYZSlice[2] = point.x;
		Instance()->mYZSlice[3] = point.y;
		break;
	default:
		assert(false);
	}

	if (postEvent)
		OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_BOX_CHANGE));
}

Vector2 < float > OmStateManager::GetViewSliceMax(ViewType plane)
{
	switch (plane) {
	case XY_VIEW:
		return Vector2 < float >(&Instance()->mXYSlice[2]);
	case XZ_VIEW:
		return Vector2 < float >(&Instance()->mXZSlice[2]);
	case YZ_VIEW:
		return Vector2 < float >(&Instance()->mYZSlice[2]);
	default:
		assert(false);
	}
}

/*
 *	Set/Get depth of view slice.
 */
void OmStateManager::SetViewSliceDepth(ViewType plane, float depth, bool postEvent)
{
	switch (plane) {
	case XY_VIEW:
		Instance()->mXYSlice[4] = depth;
		break;
	case XZ_VIEW:
		Instance()->mXZSlice[4] = depth;
		break;
	case YZ_VIEW:
		Instance()->mYZSlice[4] = depth;
		break;
	default:
		assert(false);
	}

	if (postEvent)
		OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_BOX_CHANGE));
}

float OmStateManager::GetViewSliceDepth(ViewType plane)
{
	switch (plane) {
	case XY_VIEW:
		return Instance()->mXYSlice[4];
	case XZ_VIEW:
		return Instance()->mXZSlice[4];
	case YZ_VIEW:
		return Instance()->mYZSlice[4];
	default:
		assert(false);
	}
}

/*
 *	Set/Get zoom level.
 */
void OmStateManager::SetZoomLevel(Vector2 < int >zoom)
{
	Instance()->zoom_level = zoom;

	OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_POS_CHANGE));
}

Vector2 < int > OmStateManager::GetZoomLevel()
{
	return Instance()->zoom_level;
}

/*
 *	Set/Get pan distance.
 */
void OmStateManager::SetPanDistance(ViewType plane, Vector2 < int >pan, bool postEvent)
{
	switch (plane) {
	case XY_VIEW:
		Instance()->mXYPan[0] = pan.x;
		Instance()->mXYPan[1] = pan.y;
		break;
	case XZ_VIEW:
		Instance()->mXZPan[0] = pan.x;
		Instance()->mXZPan[1] = pan.y;
		break;
	case YZ_VIEW:
		Instance()->mYZPan[0] = pan.x;
		Instance()->mYZPan[1] = pan.y;
		break;
	default:
		assert(false);
	}

	if (postEvent)
		OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_POS_CHANGE));
}

Vector2 < int > OmStateManager::GetPanDistance(ViewType plane)
{
	switch (plane) {
	case XY_VIEW:
		return Vector2 < int >(Instance()->mXYPan[0], Instance()->mXYPan[1]);
	case XZ_VIEW:
		return Vector2 < int >(Instance()->mXZPan[0], Instance()->mXZPan[1]);
	case YZ_VIEW:
		return Vector2 < int >(Instance()->mYZPan[0], Instance()->mYZPan[1]);
	default:
		assert(false);
	}
}

/*	
 *	Enable/disable orthogonal slice.
 */
void OmStateManager::SetSliceState(OmSlicePlane plane, bool enabled)
{
	switch (plane) {

	case SLICE_XY_PLANE:
		Instance()->mXYSliceEnabled = enabled;
		break;

	case SLICE_YZ_PLANE:
		Instance()->mYZSliceEnabled = enabled;
		break;

	case SLICE_XZ_PLANE:
		Instance()->mXZSliceEnabled = enabled;
		break;

	default:
		assert(false);
	}
}

/*
 *	Sets the data format for the slice image data.  This will automatically clear
 *	any image data previously set.
 */
void OmStateManager::SetViewSliceDataFormat(int bytesPerSample, int samplesPerPixel)
{
	//debug("genone","OmStateManager::SetViewSliceDataFormat");

	SetViewSlice(SLICE_XY_PLANE, Vector3 < int >::ZERO, NULL);
	SetViewSlice(SLICE_YZ_PLANE, Vector3 < int >::ZERO, NULL);
	SetViewSlice(SLICE_XZ_PLANE, Vector3 < int >::ZERO, NULL);

	Instance()->mViewSliceBytesPerSample = bytesPerSample;
	//Instance()->mViewSliceSamplesPerPixel = samplesPerPixel;      now single sample per pixel system
}

/*
 *	Sets slice dimensions and copies image data (deletes old image data if necessary).  
 *	note: Uses previously specified format to perform deep copy of given data.
 */
void OmStateManager::SetViewSlice(const OmSlicePlane plane, const Vector3 < int >&dim, unsigned char *p_data)
{
	//debug("genone","OmStateManager::SetViewSlice");

	//get size of image data
	unsigned int data_size = Instance()->mViewSliceBytesPerSample * dim.x * dim.y * dim.z;
	//alloc and copy
	unsigned char *p_data_copy = new unsigned char[data_size];
	memcpy(p_data_copy, p_data, data_size);

	switch (plane) {

	case SLICE_XY_PLANE:
		Instance()->mViewSliceDimXY = dim;
		if (Instance()->mViewSliceDataXY)
			delete Instance()->mViewSliceDataXY;
		Instance()->mViewSliceDataXY = p_data_copy;
		break;

	case SLICE_YZ_PLANE:
		Instance()->mViewSliceDimYZ = dim;
		if (Instance()->mViewSliceDataYZ)
			delete Instance()->mViewSliceDataYZ;
		Instance()->mViewSliceDataYZ = p_data_copy;
		break;

	case SLICE_XZ_PLANE:
		Instance()->mViewSliceDimXZ = dim;
		if (Instance()->mViewSliceDataXZ)
			delete Instance()->mViewSliceDataXZ;
		Instance()->mViewSliceDataXZ = p_data_copy;
		break;

	default:
		assert(false);
	}
}

#pragma mark
#pragma mark System Mode
/////////////////////////////////
///////          System Mode

OmSystemMode OmStateManager::GetSystemMode()
{
	return Instance()->mSystemMode;
}

void OmStateManager::SetSystemMode(const OmSystemMode new_mode)
{

	//return if no change
	if (new_mode == Instance()->mSystemMode)
		return;

	//set new mode
	Instance()->mSystemMode = new_mode;

	//post change event
	OmEventManager::PostEvent(new OmSystemModeEvent(OmSystemModeEvent::SYSTEM_MODE_CHANGE));
}

#pragma mark
#pragma mark Tool Mode
/////////////////////////////////
///////          Tool Mode

OmToolMode OmStateManager::GetToolMode()
{
	return Instance()->mToolMode;
}

void OmStateManager::SetToolMode(const OmToolMode new_mode)
{
	//return if no change
	if (new_mode == Instance()->mToolMode)
		return;

	//set new mode
	Instance()->mToolMode = new_mode;

	//post tool mode change
	OmEventManager::PostEvent(new OmToolModeEvent(OmToolModeEvent::TOOL_MODE_CHANGE));
}

#pragma mark
#pragma mark UndoStack
/////////////////////////////////
///////          UndoStack

QUndoStack *OmStateManager::GetUndoStack()
{

	if (NULL == Instance()->mpUndoStack) {
		Instance()->mpUndoStack = new QUndoStack();
	}

	return Instance()->mpUndoStack;
}

void OmStateManager::PushUndoCommand(QUndoCommand * cmd)
{
	GetUndoStack()->push(cmd);
}

void OmStateManager::ClearUndoStack()
{
	GetUndoStack()->clear();
}

static QGLWidget *sharedwidget = NULL;

#pragma mark
#pragma mark View3d Context
/////////////////////////////////
///////          View3d Context

/*
 *	Create the primary QGLContext for the View3d system.
 */
void OmStateManager::CreatePrimaryView3dWidget()
{
	//create primary widget
	Instance()->mpPrimaryView3dWidget = new QGLWidget();

	//set primary widget gl format properties
	Instance()->mpPrimaryView3dWidget->setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));
}

/*
 *	Returns pointer to the primary widget.
 */
const QGLWidget *OmStateManager::GetPrimaryView3dWidget()
{
	//create if primary widget does not yet exist
	if (Instance()->mpPrimaryView3dWidget == NULL)
		CreatePrimaryView3dWidget();

	return Instance()->mpPrimaryView3dWidget;
}

/*
 *	Returns a pointer to a QGLContext that is shared with the primary QGLContext.  
 */
QGLContext *OmStateManager::GetSharedView3dContext()
{

	return NULL;
	//create primary widget if it does not exist yet
	if (Instance()->mpPrimaryView3dWidget == NULL)
		CreatePrimaryView3dWidget();

	//get primary context
	const QGLContext *p_primary_context = Instance()->mpPrimaryView3dWidget->context();

	//make isntance of a shared context
	QGLContext *p_shared_context = new QGLContext(p_primary_context->format(),
						      p_primary_context->device());

	//create context that is shared with primary
	p_shared_context->create(p_primary_context);

	//assert context is valid and is sharing
	assert(p_shared_context->isValid() && "Shared context not valid.");
	assert(p_shared_context->isSharing() && "New shared context is not sharing");

	return p_shared_context;
}

/*
 *	Makes given QGLContext current. 
 */
void OmStateManager::MakeContextCurrent(QGLContext * pContext)
{
	//pContext->makeCurrent();
}

/*
 *	Returns a pointer to a QGLContext that is shared with the primary QGLContext.  
 */
QGLContext *OmStateManager::GetSharedView2dContext(const QGLContext * pContext)
{

//      //create primary widget if it does not exist yet
//      if(Instance()->mpPrimaryView3dWidget == NULL) 
//              CreatePrimaryView3dWidget();
//      
//      //get primary context
//      const QGLContext* p_primary_context = Instance()->mpPrimaryView3dWidget->context();

#if 0
	//make isntance of a shared context
	QGLContext *p_shared_context = new QGLContext(pContext->format(),
						      pContext->device());

	//create context that is shared with primary
	p_shared_context->create(pContext);

	//assert context is valid and is sharing
	assert(p_shared_context->isValid() && "Shared context not valid.");
	assert(p_shared_context->isSharing() && "New shared context is not sharing");

	return p_shared_context;
#endif
	return NULL;
}

#pragma mark
#pragma mark Transparency State
/////////////////////////////////
///////          Transparency State

float OmStateManager::GetTransparencyAlpha()
{
	return Instance()->TRANSPARANCY_ALPHA;
}

void OmStateManager::SetTransparencyAlpha(float alpha)
{
	Instance()->TRANSPARANCY_ALPHA = alpha;
}


unsigned int OmStateManager::getMyBackoff()
{
	return Instance()->myBackoff;
}

void OmStateManager::setMyBackoff( unsigned int val)
{
	Instance()->myBackoff = val;
}

