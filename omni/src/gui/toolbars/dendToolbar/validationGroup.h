#ifndef VALIDATION_GROUP_H
#define VALIDATION_GROUP_H

#include <QtGui>

class DendToolBar;
class GroupButtonAdd;
class GroupButtonDelete;
class GroupButtonTag;
class ShowValidatedButton;
class SegmentationDataWrapper;
class OmViewGroupState;

class ValidationGroup : public QGroupBox {
 Q_OBJECT
 public:
	ValidationGroup(DendToolBar *);
	QString getGroupNameFromGUI();
	bool isShowValidChecked();

	SegmentationDataWrapper getSegmentationDataWrapper();
	OmViewGroupState * getViewGroupState();

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
