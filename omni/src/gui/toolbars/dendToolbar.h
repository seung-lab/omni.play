#ifndef DEND_TOOLBAR_H
#define DEND_TOOLBAR_H

#include "common/omCommon.h"
#include "project/omProject.h"
#include "gui/toolbars/dendToolbar.h"
#include <QtGui>

class MainWindow;
class OmViewGroupState;

class DendToolBar : public QWidget
{
	Q_OBJECT

 public:

	DendToolBar( MainWindow * mw );
	void setupToolbarInitially();
	void updateReadOnlyRelatedWidgets();
	void updateGuiFromProjectLoadOrOpen(OmViewGroupState *);
	void SystemModeChangeEvent();
	void updateGui();

        void SetSplittingOff();

	static bool GetShowGroupsMode();

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
        void addGroup();
        void mapColors();

 private:
	MainWindow * mMainWindow;
	OmViewGroupState * mViewGroupState;
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

	QPushButton * addGroupAct;
        QPushButton * colorMapAct;
	
	void setToolbarDisabled();
	void resetTool( QAction* tool, const bool enabled );
	void resetTools( const bool enabled );

};

#endif
