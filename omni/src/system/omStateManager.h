#ifndef OM_STATE_MANAGER_H
#define OM_STATE_MANAGER_H

/*
 *	Manages data structures that are shared between various parts of the system.  Making centralized
 *	changes in the StateManager will send events that cause the other interested systems to be
 *	notified and synchronized.
 *
 *	Brett Warne - bwarne@mit.edu - 3/14/09
 */

#include "common/omCommon.h"

#include <QSize>

class QUndoStack;
class QUndoCommand;
class QGLWidget;
class QGLContext;
class MyInspectorWidget;
class MainWindow;
class DendToolBar;

enum OmSlicePlane { SLICE_XY_PLANE, SLICE_XZ_PLANE, SLICE_YZ_PLANE };
enum OmSystemMode { NAVIGATION_SYSTEM_MODE, EDIT_SYSTEM_MODE, DEND_MODE };
enum OmToolMode { SELECT_MODE,
		  PAN_MODE,
		  CROSSHAIR_MODE,
		  ZOOM_MODE, 
		  ADD_VOXEL_MODE, 
		  SUBTRACT_VOXEL_MODE, 
		  SELECT_VOXEL_MODE,
		  FILL_MODE, 
		  VOXELIZE_MODE,
};

class OmStateManager : boost::noncopyable {

public:
	
	static OmStateManager* Instance();
	static void Delete();
	static void UpdateStatusBar( QString msg );

	//project
	static const string& GetProjectFileName();
	static void SetProjectFileName(const string &);
	static const string& GetProjectDirectoryPath();
	static void SetProjectDirectoryPath(const string &);
	
	
	//system mode
	static OmSystemMode GetSystemMode();
	static OmSystemMode GetSystemModePrev();
	static void SetSystemMode(const OmSystemMode mode);
	static void SetSystemModePrev();
	
	//tool mode
	static OmToolMode GetToolMode();
	static void SetToolMode(const OmToolMode mode);
	
	//undostack
	static QUndoStack* GetUndoStack();
	static void PushUndoCommand(QUndoCommand *);
	static void ClearUndoStack();
	
	
	//view3d context
	static void CreatePrimaryView3dWidget();
	static const QGLWidget* GetPrimaryView3dWidget();
	static QGLContext* GetSharedView3dContext();
	
	//view2d context
	static QGLContext* GetSharedView2dContext(const QGLContext *pContext);
	
	//central transparency value (to avoid gross pref lookups)
	static float GetTransparencyAlpha();
	static void SetTransparencyAlpha(float);
	
	static void setOmniExecutableAbsolutePath( QString abs_path );
	static QString getOmniExecutableAbsolutePath();

	static QString getPID();
	static QString getHostname();
	static bool getParallel();
	static void setParallel(bool parallel);

	static void setInspector( MyInspectorWidget * miw );
	static void setMainWindow( MainWindow * mw );
	static void setDendToolBar( DendToolBar * dtb);

	static QSize getViewBoxSizeHint();

private:
	OmStateManager();
	~OmStateManager();

	//singleton
	static OmStateManager* mspInstance;
	
	//project
	string mProjectFileName;
	string mProjectDirectoryPath;
	
	//edit selection
	Vector3< OmId > mEditSelection;	//volume, segmentation, segment
	set< DataCoord > mEditSelectionCoords;
	
	//system mode
	OmSystemMode mSystemMode;
	OmSystemMode mSystemModePrev;

	//tool mode
	OmToolMode mToolMode;
	
	//undostack
	QUndoStack *mpUndoStack;
	
	//view3d context
	QGLWidget *mpPrimaryView3dWidget;
	
	//transparency
	float TRANSPARANCY_ALPHA;

	unsigned int myBackoff;

	QString omniExecPathAbsolute;
	bool mParallel;

	MyInspectorWidget * inspectorWidget;
	MainWindow * mainWindow;
	DendToolBar * dendToolBar;
};

#endif
