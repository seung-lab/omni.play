#ifndef MENUBAR_H
#define MENUBAR_H

#include <QtGui>

#include "gui/recentFileList.h"

class MainWindow;

class MenuBar : public QWidget
{
	Q_OBJECT
		
 public:
	MenuBar( MainWindow * mw );
	void updateReadOnlyRelatedWidgets();
	QMenu * getWindowMenu();
	QAction * getOpenCacheMonitorAct();
	void addRecentFile( QString fname );

 private:
	MainWindow * mMainWindow;

	void createActions();
	void createMenus();

	QMenu *fileMenu;
	QMenu *editMenu;
	QMenu *projectMenu;
	QMenu *toolMenu;
	QMenu *windowMenu;
	
	QAction *newAct;
	QAction *openAct;
	QAction *saveAct;
	QAction *closeAct;
	QAction *quitAct;
		
	QAction *editPreferencesAct;
	QAction *editLocalPreferencesAct;

	QAction *addChannelAct;
	QAction *addSegmentationAct;
		
	QAction *openOmniInspector;
	QAction *openUndoViewAct;
	QAction *openCacheMonitorAct;
	QAction *openGroupsTableAct;
		
	QAction *open3DAct;

	RecentFileList recentFiles;
};

#endif
