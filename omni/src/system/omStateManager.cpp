#include "common/omDebug.h"
#include "events/omToolModeEvent.h"
#include "events/omView3dEvent.h"
#include "events/omViewEvent.h"
#include "gui/mainwindow.h"
#include "gui/myInspectorWidget.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
#include "omEventManager.h"
#include "omStateManager.h"
#include "project/omProject.h"
#include "system/omEvents.h"
#include "system/omLocalPreferences.h"

#include <QHostInfo>

//undostack
#include <QUndoStack>
#include <QApplication>

//view3d context
#include <QtOpenGL/qgl.h>
#include <QtOpenGL/QGLFormat>

//init instance pointer
OmStateManager *OmStateManager::mspInstance = 0;

OmStateManager::OmStateManager()
{
	//view3d
	mpPrimaryView3dWidget = NULL;
	mpUndoStack = NULL;

	mPrevToolMode = PAN_MODE;
	mCurToolMode = PAN_MODE;

	mParallel = false;

	inspectorWidget = NULL;
	mainWindow = NULL;
	dendToolBar = NULL;
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
		mspInstance = new OmStateManager();
	}
	return mspInstance;
}

/*
 *	Static destructor
 */
void OmStateManager::Delete()
{
	if (mspInstance) {
		delete mspInstance;
		mspInstance = NULL;
	}
}

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

void OmStateManager::setOmniExecutableAbsolutePath( QString abs_path ) 
{
	Instance()->omniExecPathAbsolute = abs_path;
}

QString OmStateManager::getOmniExecutableAbsolutePath() 
{
	return Instance()->omniExecPathAbsolute;
}
#if WIN32
#define getpid() 0
#define pid_t int
#endif
QString OmStateManager::getPID()
{
        static char pidstr[6] = {0};
        pid_t pid;
        int i;

        if (pidstr[0] == 0) {
                pid = getpid();
                for(i = 0; i < 5; i++) {
                        pidstr[4 - i] = (pid % 10) + '0';
                        pid /= 10;
                }
                pidstr[5] = 0;
        }

	return QString (pidstr);
}

QString OmStateManager::getHostname()
{
#ifdef WIN32
	return QString("localhost");
#else
	return QHostInfo::localHostName();
#endif
}

bool OmStateManager::getParallel()
{
	return Instance()->mParallel;
}

void OmStateManager::setParallel(bool parallel)
{
	// This has a race condition in that others migth have been acting on this
	// value while it is changing, which could lead to a crash.
	Instance()->mParallel = parallel;
}

/////////////////////////////////
///////          Tool Mode

OmToolMode OmStateManager::GetToolMode()
{
	return Instance()->mCurToolMode;
}

void OmStateManager::SetToolModeAndSendEvent(const OmToolMode tool)
{
	if (tool == Instance()->mCurToolMode){
		return;
	}

	Instance()->mPrevToolMode = Instance()->mCurToolMode;
	Instance()->mCurToolMode = tool;

	OmEvents::ToolChange();
}

void OmStateManager::SetOldToolModeAndSendEvent()
{
	std::swap(Instance()->mPrevToolMode, Instance()->mCurToolMode );

	OmEvents::ToolChange();
}

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

void OmStateManager::UndoUndoCommand()
{
	GetUndoStack()->undo();
}

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


void OmStateManager::setInspector( MyInspectorWidget * miw )
{
	Instance()->inspectorWidget = miw;
}

void OmStateManager::setMainWindow( MainWindow * mw )
{
	Instance()->mainWindow = mw;
}

QSize OmStateManager::getViewBoxSizeHint()
{
	QWidget * mw = Instance()->mainWindow;
	if(NULL == mw){
		mw = QApplication::activeWindow();
		if(NULL == mw){
			printf("warning: assuming window size is 1000x640\n");
			return QSize(1000, 640);
		}
	}
	int w = mw->width();
	int h = mw->height();
	
	if( Instance()->inspectorWidget != NULL ){
		w -= Instance()->inspectorWidget->width();
	}

	if( Instance()->dendToolBar != NULL ){
		w -= Instance()->dendToolBar->width();
	}

	return QSize( w, h );
}

void OmStateManager::setDendToolBar( DendToolBar * dtb)
{
	Instance()->dendToolBar = dtb;
}

void OmStateManager::UpdateStatusBar( QString msg )
{
	Instance()->mainWindow->updateStatusBar( msg );
}

void OmStateManager::SetViewDrawable(ViewType viewType, vector<Drawable*> &drawable)
{
	switch(viewType){
	case XY_VIEW:
		Instance()->mDrawableXY = drawable;
		break;
	case XZ_VIEW:
		Instance()->mDrawableXZ = drawable;
		break;
	case YZ_VIEW:
		Instance()->mDrawableYZ = drawable;
		break;
	default:
		assert(false);
	}
}

vector<Drawable*> 
OmStateManager::GetViewDrawable(ViewType viewType)
{
	switch(viewType){
	case XY_VIEW:
		return Instance()->mDrawableXY;
	case XZ_VIEW:
		return Instance()->mDrawableXZ;
	case YZ_VIEW:
		return Instance()->mDrawableYZ;
	default:
		assert(false);
	}	
}
