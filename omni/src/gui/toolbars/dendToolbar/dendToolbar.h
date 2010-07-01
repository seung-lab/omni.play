#ifndef DEND_TOOLBAR_H
#define DEND_TOOLBAR_H

#include "common/omCommon.h"

#include <QtGui>

class MainWindow;
class SplitButton;
class OmViewGroupState;
class SegmentationDataWrapper;

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
	SegmentationDataWrapper getSegmentationDataWrapper();
	static bool GetShowGroupsMode();

 private slots:
	void ChangeModeModify(bool checked);
	void autoBreakChecked();
	void increaseBreakThreshold();
	void decreaseBreakThreshold();
	void breakThresholdChanged();
	void increaseDustThreshold();
	void decreaseDustThreshold();
	void dustThresholdChanged();

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

	void addToBreakThreshold(float num);
	void addToDustThreshold(float num);

	void setBreakThresholdValue();
	void setDustThresholdValue();
	OmId getSegmentationID();

	// Actions
	SplitButton * splitButton;
	QCheckBox * autoBreakCheckbox;
	QLabel * thresholdLabel;
	QLabel* breakThresholdLabel;

	QLineEdit* mBreakThreshold;
	QPushButton * increaseBreakThresholdAct;
	QPushButton * decreaseBreakThresholdAct;

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


};

#endif
