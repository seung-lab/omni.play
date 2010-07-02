#ifndef DEND_TOOLBAR_H
#define DEND_TOOLBAR_H

#include "common/omCommon.h"

#include <QtGui>

class SplitButton;
class MainWindow;
class OmViewGroupState;
class SegmentationDataWrapper;

class DendToolBar : public QWidget {
 Q_OBJECT
 public:
	DendToolBar( MainWindow * mw );
	void updateGuiFromProjectLoadOrOpen(OmViewGroupState *);
	void updateGui();

        void SetSplittingOff();

	OmViewGroupState * getViewGroupState(){ return mViewGroupState; }
	SegmentationDataWrapper getSegmentationDataWrapper();
	static bool GetShowGroupsMode();

 private slots:
	void autoBreakChecked();

        void addGroup();
        void deleteGroup();
        void mapColors();
        void changeMapColors();
        void specialGroupAdd();

 private:
	MainWindow * mMainWindow;
	OmViewGroupState * mViewGroupState;
	QToolBar * dendToolBar;

	SplitButton * splitButton;

	void createToolbarActions();
	void addToolbars();

	QCheckBox * autoBreakCheckbox;
	QLabel * thresholdLabel;

	QPushButton * addGroupAct;
	QPushButton * deleteGroupAct;
        QPushButton * colorMapAct;
	QButtonGroup * validGroup;
        QRadioButton * showValid;
        QRadioButton * dontShowValid;
        QPushButton * specialGroupAct;
	QLineEdit* mGroupName;

	OmId getSegmentationID();
};

#endif
