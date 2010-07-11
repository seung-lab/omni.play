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
class Drawable;

class OmStateManager : boost::noncopyable {

public:
	
	static OmStateManager* Instance();
	static void Delete();

	static void UpdateStatusBar( const QString & msg );

	//project
	static const string& GetProjectFileName();
	static void SetProjectFileName(const string &);
	static const string& GetProjectDirectoryPath();
	static void SetProjectDirectoryPath(const string &);
		
	//tool mode
	static OmToolMode GetToolMode();
	static void SetToolModeAndSendEvent(const OmToolMode mode);
	static void SetOldToolModeAndSendEvent();

	//undostack
	static QUndoStack* GetUndoStack();
	static void PushUndoCommand(QUndoCommand *);
	static void ClearUndoStack();
	static void UndoUndoCommand();
	
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

	static void SetViewDrawable(ViewType viewType, vector<Drawable*> & drawable);
	static vector<Drawable*> GetViewDrawable(ViewType viewType);

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
	
	//tool mode
	OmToolMode mCurToolMode;
	OmToolMode mPrevToolMode;
	
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

	vector<Drawable*> mDrawableXY;
	vector<Drawable*> mDrawableXZ;
	vector<Drawable*> mDrawableYZ;
};

#endif
