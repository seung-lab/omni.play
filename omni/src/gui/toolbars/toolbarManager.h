#ifndef TOOLBAR_MANAGER_H
#define TOOLBAR_MANAGER_H

#include <QtGui>

class MainWindow;
class NavAndEditToolBars;
class DendToolBar;
class OmViewGroupState;

class ToolBarManager : public QWidget
{
	Q_OBJECT
		
 public:
	ToolBarManager( MainWindow * mw );
	virtual void setupToolbarInitially();
	virtual void updateReadOnlyRelatedWidgets();
	virtual void updateGuiFromProjectLoadOrOpen(OmViewGroupState *);
	virtual void SystemModeChangeEvent();

 private:
	MainWindow * mMainWindow;
	NavAndEditToolBars * navAndEditToolBars;
	DendToolBar * dendToolBar;
};

#endif
