#ifndef DEND_TOOLBAR_H
#define DEND_TOOLBAR_H

#include "common/omCommon.h"
#include "project/omProject.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
#include <QtGui>

class MainWindow;
class SplitButton;
class OmViewGroupState;

class DendToolBar : public QWidget
{
	Q_OBJECT

 public:

	DendToolBar( MainWindow * mw );
	void setupToolbarInitially();
	void updateReadOnlyRelatedWidgets();
	void updateGuiFromProjectLoadOrOpen(OmViewGroupState *);
	void updateGui();

        void SetSplittingOff();

	OmViewGroupState * getViewGroupState(){ return mViewGroupState; }

	static bool GetShowGroupsMode();

 private slots:
	void ChangeModeModify(bool checked);
	void autoBreakChecked();
	void increaseThreshold();
	void decreaseThreshold();
	void increaseBreakThreshold();
	void decreaseBreakThreshold();
	void breakThresholdChanged();
	void increaseDustThreshold();
	void decreaseDustThreshold();
	void dustThresholdChanged();
	void thresholdChanged();
	void join();
	void toggledShatter();
	void toggledHint();
        void addGroup();
        void deleteGroup();
        void mapColors();
        void changeMapColors();
        void specialGroupAdd();

 private:
	MainWindow * mMainWindow;
	OmViewGroupState * mViewGroupState;
	QToolBar * dendToolBar;

	void createToolbar();
	void createToolbarActions();
	void addToolbars();
	void addToThreshold(float num);
	void addToBreakThreshold(float num);
	void addToDustThreshold(float num);
	void setThresholdValue();
	void setBreakThresholdValue();
	void setDustThresholdValue();
	OmId getSegmentationID();

	// Actions
	SplitButton * splitButton;
	QCheckBox * autoBreakCheckbox;
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
	QPushButton * deleteGroupAct;
        QPushButton * colorMapAct;
	QButtonGroup * validGroup;
        QRadioButton * showValid;
        QRadioButton * dontShowValid;
        QPushButton * specialGroupAct;
	QLineEdit* mGroupName;

	QLineEdit* mDustThreshold;
	QPushButton * increaseDustThresholdAct;
	QPushButton * decreaseDustThresholdAct;
	
	void setToolbarDisabled();
	void resetTool( QAction* tool, const bool enabled );
	void resetTools( const bool enabled );

	void haveSegmentationChangeThreshold( const float threshold );
};

#endif
