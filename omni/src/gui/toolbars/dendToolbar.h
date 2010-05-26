#ifndef DEND_TOOLBAR_H
#define DEND_TOOLBAR_H

#include "common/omCommon.h"
#include "project/omProject.h"
#include <QtGui>

class MainWindow;

class DendToolBar : public QWidget
{
	Q_OBJECT

 public:
	static bool GetShatterMode();
	DendToolBar( MainWindow * mw );
	void setupToolbarInitially();
	void updateReadOnlyRelatedWidgets();
	void updateGuiFromProjectLoadOrOpen();
	void SystemModeChangeEvent();
	void updateGui();

 private slots:
	void ChangeModeModify(bool checked);
	void toolbarSplit(bool checked);
	void split();
	void increaseThreshold();
	void decreaseThreshold();
	void thresholdChanged();
	void join();
	void toggledShatter();
	void toggledHint();

 private:
	MainWindow * mMainWindow;

	QToolBar * dendToolBar;

	void createToolbar();
	void createToolbarActions();
	void addToolbars();
	void addToThreshold(float num);

	void setThresholdValue();

	// Actions
	QAction * toolbarSplitAct;
	QAction * decreaseThresholdAct;
	QLabel * thresholdLabel;
	QLineEdit* mThreshold;
	QAction * increaseThresholdAct;
	QAction * joinAct;
	QAction * toolbarShatterAct;
	QAction * mergeHintAct;
	QLineEdit* mHint;
	
	void setToolbarDisabled();
	void resetTool( QAction* tool, const bool enabled );
	void resetTools( const bool enabled );

	OmId mSeg;
};

#endif
