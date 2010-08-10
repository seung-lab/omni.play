#ifndef VALIDATION_GROUP_H
#define VALIDATION_GROUP_H

#include <QtGui>
#include "gui/widgets/omWidget.hpp"

class DendToolBar;
class GroupButtonAdd;
class GroupButtonDelete;
class GroupButtonTag;
class ShowValidatedButton;
class SegmentationDataWrapper;
class OmViewGroupState;

class ValidationGroup : public OmWidget {
 Q_OBJECT
 public:
	ValidationGroup(DendToolBar *);
	QString getGroupNameFromGUI();
	bool isShowValidChecked();

	SegmentationDataWrapper getSegmentationDataWrapper();
	OmViewGroupState * getViewGroupState();

	QString getName(){ return "Validation"; }

 private slots:
        void changeMapColors();

 private:
	DendToolBar *const mDendToolBar;

	QButtonGroup * validGroup;
        QRadioButton * showValid;
        QRadioButton * dontShowValid;
	QLineEdit* mGroupName;

	GroupButtonAdd * groupButtonAdd;
	GroupButtonDelete * groupButtonDelete;
	GroupButtonTag * groupButtonTag;
	ShowValidatedButton * showValidatedButton;
};

#endif
