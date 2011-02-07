#ifndef VALIDATION_GROUP_H
#define VALIDATION_GROUP_H

#include <QtGui>
#include "gui/widgets/omWidget.hpp"

class DendToolBar;
class SetValid;
class SetNotValid;
class SetUncertain;
class SetNotUncertain;
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

	SegmentationDataWrapper GetSegmentationDataWrapper();
	OmViewGroupState * getViewGroupState();

	QString getName(){ return "Validation"; }

private Q_SLOTS:
	void changeMapColors();

private:
	DendToolBar *const mDendToolBar;

	QButtonGroup * validGroup;
	QRadioButton * showValid;
	QRadioButton * dontShowValid;
	QLineEdit* mGroupName;

	SetValid* setSelectionValid;
	SetNotValid* setSelectionNotValid;

	SetUncertain* setSelectionUncertain;
	SetNotUncertain* setSelectionNotUncertain;

	GroupButtonTag * groupButtonTag;
	ShowValidatedButton * showValidatedButton;

	QWidget* addSelectedSegmentButtons();
};

#endif
