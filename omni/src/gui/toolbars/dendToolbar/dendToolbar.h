#ifndef DEND_TOOLBAR_H
#define DEND_TOOLBAR_H

#include "common/omCommon.h"

#include <QtGui>

class MainWindow;
class BreakButton;
class SplitButton;
class OmViewGroupState;
class SegmentationDataWrapper;

class DendToolBar : public QWidget {
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

	void join();
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

	OmId getSegmentationID();

	// Actions
	SplitButton * splitButton;
	BreakButton * breakButton;

	QCheckBox * autoBreakCheckbox;
	QLabel * thresholdLabel;


	QPushButton * joinAct;

	QPushButton * addGroupAct;
	QPushButton * deleteGroupAct;
        QPushButton * colorMapAct;
	QButtonGroup * validGroup;
        QRadioButton * showValid;
        QRadioButton * dontShowValid;
        QPushButton * specialGroupAct;
	QLineEdit* mGroupName;

	void setToolbarDisabled();
	void resetTool( QAction* tool, const bool enabled );
	void resetTools( const bool enabled );
};

#endif
