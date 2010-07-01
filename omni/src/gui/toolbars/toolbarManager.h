#ifndef TOOLBAR_MANAGER_H
#define TOOLBAR_MANAGER_H

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
	virtual void setupToolbarInitially();
	virtual void updateReadOnlyRelatedWidgets();
	virtual void updateGuiFromProjectLoadOrOpen(OmViewGroupState *);

	void SetSplittingOff();

 private:
	MainWindow * mMainWindow;
	MainToolbar * mainToolbar;
	DendToolBar * dendToolBar;
};

#endif
