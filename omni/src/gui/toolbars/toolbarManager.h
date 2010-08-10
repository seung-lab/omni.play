#ifndef TOOLBAR_MANAGER_H
#define TOOLBAR_MANAGER_H

#include "common/omCommon.h"

#include <QtGui>

class MainWindow;
class MainToolbar;
class DendToolBar;
class OmViewGroupState;

class ToolBarManager : public QWidget
{
	Q_OBJECT

public:
	ToolBarManager( MainWindow * mw );
	void setupToolbarInitially();
	void updateReadOnlyRelatedWidgets();
	void updateGuiFromProjectLoadOrOpen(OmViewGroupState *);
	void windowResized(QPoint oldPos);
	void windowMoved(QPoint oldPos);

	void SetSplittingOff();
	void setTool(const OmToolMode tool);

 private:
	MainWindow * mMainWindow;
	MainToolbar * mainToolbar;
	DendToolBar * dendToolBar;
};

#endif
