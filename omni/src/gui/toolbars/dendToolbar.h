#ifndef DEND_TOOLBAR_H
#define DEND_TOOLBAR_H

#include <QtGui>

class MainWindow;

class DendToolBar : public QWidget
{
	Q_OBJECT

 public:
	DendToolBar( MainWindow * mw );
	void setupToolbarInitially();
	void updateReadOnlyRelatedWidgets();
	void updateGuiFromPorjectLoadOrOpen();
	void SystemModeChangeEvent();

 private slots:
	void ChangeModeModify(bool checked);
	void toolbarSplit(bool checked);
	void split();

 private:
	MainWindow * mMainWindow;

	QToolBar * dendToolBar;

	// new toolbar
	void createToolbar();
	void createToolbarActions();
	void addToolbars();

	QAction *toolbarSplitAct;

	void setToolbarDisabled();
	void resetTool( QAction* tool, const bool enabled );
	void resetTools( const bool enabled );

};

#endif
