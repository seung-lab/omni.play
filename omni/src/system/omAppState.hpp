#ifndef OM_APP_STATE_HPP
#define OM_APP_STATE_HPP

#include "common/omCommon.h"
#include "zi/omUtility.h"

#include <QSize>
#include <QUndoStack>
#include <QGLWidget>
#include <QGLContext>

class MyInspectorWidget;
class MainWindow;
class DendToolBar;

class OmAppState : private om::singletonBase<OmAppState> {
private:
	MyInspectorWidget* inspectorWidget_;
	MainWindow* mainWindow_;
	DendToolBar* dendToolBar_;

public:
	static QString GetPID();
	static QString GetHostname();

	static void SetInspector(MyInspectorWidget* miw);
	static void SetMainWindow(MainWindow* mw);
	static void SetDendToolBar(DendToolBar* dtb);

	static void UpdateStatusBar(const QString & msg);

	static QSize GetViewBoxSizeHint();

private:
	OmAppState()
		: inspectorWidget_(NULL)
		, mainWindow_(NULL)
		, dendToolBar_(NULL)
	{}

	~OmAppState(){};

	friend class zi::singleton<OmAppState>;
};

#endif
