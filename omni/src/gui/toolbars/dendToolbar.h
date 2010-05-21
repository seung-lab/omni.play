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
	static bool GetSplitMode(OmId & seg, OmId & segment);
	static void SetSplitMode(OmId seg, OmId segment);
	static void SetSplitMode(bool onoroff);



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
	void increaseBreakThreshold();
	void decreaseBreakThreshold();
	void breakThresholdChanged();
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
	void addToBreakThreshold(float num);
	void setThresholdValue();
	void setBreakThresholdValue();

	// Actions
	QPushButton * toolbarSplitAct;
	QPushButton * decreaseThresholdAct;
	QLabel * thresholdLabel;
	QLabel* breakThresholdLabel;
	QLineEdit* mThreshold;
	QLineEdit* mBreakThreshold;
	QPushButton * increaseBreakThresholdAct;
	QPushButton * decreaseBreakThresholdAct;

	QPushButton * increaseThresholdAct;
	QPushButton * joinAct;
	QPushButton * toolbarShatterAct;
	QPushButton * mergeHintAct;
	QLineEdit* mHint;
	
	void setToolbarDisabled();
	void resetTool( QAction* tool, const bool enabled );
	void resetTools( const bool enabled );

};

#endif
