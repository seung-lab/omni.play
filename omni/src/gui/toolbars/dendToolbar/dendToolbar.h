#ifndef DEND_TOOLBAR_H
#define DEND_TOOLBAR_H

#include "common/omCommon.h"

#include <QtGui>

class GroupButtonAdd;
class GroupButtonDelete;
class GroupButtonTag;
class MainWindow;
class OmViewGroupState;
class SegmentationDataWrapper;
class ShowValidatedButton;
class SplitButton;

class DendToolBar : public QToolBar {
 Q_OBJECT
 public:
	DendToolBar( MainWindow * mw );
	void updateGuiFromProjectLoadOrOpen(OmViewGroupState *);
	void updateGui();

        void SetSplittingOff();

	OmViewGroupState * getViewGroupState(){ return mViewGroupState; }
	SegmentationDataWrapper getSegmentationDataWrapper();
	static bool GetShowGroupsMode();

	QString getGroupNameFromGUI();
	bool isShowValidChecked();
	
 private slots:
	void autoBreakChecked();
        void changeMapColors();

 private:
	MainWindow * mMainWindow;
	OmViewGroupState * mViewGroupState;
	QToolBar * dendToolBar;

	SplitButton * splitButton;

	void createToolbarActions();
	void addToolbars();

	QCheckBox * autoBreakCheckbox;
	QLabel * thresholdLabel;

	GroupButtonAdd * groupButtonAdd;
	GroupButtonDelete * groupButtonDelete;
	GroupButtonTag * groupButtonTag;
	ShowValidatedButton * showValidatedButton;

	QButtonGroup * validGroup;
        QRadioButton * showValid;
        QRadioButton * dontShowValid;
	QLineEdit* mGroupName;
};

#endif
