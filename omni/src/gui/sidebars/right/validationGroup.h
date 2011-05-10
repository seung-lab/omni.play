#pragma once

#include "gui/widgets/omWidget.hpp"

#include <QtGui>

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
    ValidationGroup(DendToolBar*, OmViewGroupState* vgs);
    QString getGroupNameFromGUI();
    bool isShowValidChecked();

    QString getName(){
        return "Validation";
    }

    OmViewGroupState* GetViewGroupState() const;

    SegmentationDataWrapper GetSDW() const;

private Q_SLOTS:
    void changeMapColors();

private:
    OmViewGroupState *const vgs_;

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

