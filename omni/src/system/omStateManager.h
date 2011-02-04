#ifndef OM_STATE_MANAGER_H
#define OM_STATE_MANAGER_H

/*
 *	Manages data structures that are shared between various parts of the system.
 *
 *	Brett Warne - bwarne@mit.edu - 3/14/09
 */

#include "common/omCommon.h"
#include "zi/omUtility.h"
#include "project/omProject.h"
#include "project/omProjectGlobals.h"

#include <QSize>
#include <QUndoStack>
#include <QGLWidget>
#include <QGLContext>

class MyInspectorWidget;
class MainWindow;
class DendToolBar;
class OmStateManagerImpl;

class OmStateManager : private om::singletonBase<OmStateManager> {
private:
	inline static OmStateManagerImpl& impl(){
		return OmProject::Globals().StateManagerImpl();
	}

public:
	//tool mode
	static OmToolMode GetToolMode();
	static void SetToolModeAndSendEvent(const OmToolMode mode);
	static void SetOldToolModeAndSendEvent();

	//undostack
	static QUndoStack* GetUndoStack();
	static void PushUndoCommand(QUndoCommand*);
	static void ClearUndoStack();
	static void UndoUndoCommand();

	//view3d context
	static const QGLWidget* GetPrimaryView3dWidget();

private:
	OmStateManager();
	~OmStateManager();

	friend class zi::singleton<OmStateManager>;
};

#endif
