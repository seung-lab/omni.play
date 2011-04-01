#include "system/omConnect.hpp"
#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/validationGroup.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
#include "utility/dataWrappers.h"
#include "gui/toolbars/dendToolbar/setValid.hpp"
#include "gui/toolbars/dendToolbar/setNotValid.hpp"
#include "gui/toolbars/dendToolbar/setUncertain.hpp"
#include "gui/toolbars/dendToolbar/setNotUncertain.hpp"
#include "gui/toolbars/dendToolbar/groupButtonTag.h"
#include "gui/toolbars/dendToolbar/showValidatedButton.h"
#include "viewGroup/omViewGroupState.h"

ValidationGroup::ValidationGroup(DendToolBar * d)
    : OmWidget(d)
    , mDendToolBar(d)
    , setSelectionValid(new SetValid(this))
    , setSelectionNotValid(new SetNotValid(this))
    , setSelectionUncertain(new SetUncertain(this))
    , setSelectionNotUncertain(new SetNotUncertain(this))
    , groupButtonTag(new GroupButtonTag(this))
    , showValidatedButton(new ShowValidatedButton(this))
{
    validGroup = new QButtonGroup();
    showValid = new QRadioButton("In Color");
    validGroup->addButton(showValid);
    om::connect(showValid, SIGNAL(toggled(bool)),
            this, SLOT(changeMapColors()));

    dontShowValid = new QRadioButton("As Black");
    dontShowValid->setChecked(true);
    validGroup->addButton(dontShowValid);
    om::connect(dontShowValid, SIGNAL(toggled(bool)),
            this, SLOT(changeMapColors()));

    mGroupName = new QLineEdit(this);
    mGroupName->setText("Glia");

    QGridLayout * box = new QGridLayout(this);
    box->addWidget(addSelectedSegmentButtons(),0,0,2,2);
    box->addWidget(showValidatedButton,2,0,1,2);
    box->addWidget(showValid,3,0,1,1);
    box->addWidget(dontShowValid,3,1,1,1);
    box->addWidget(groupButtonTag,4,0,1,1);
    box->addWidget(mGroupName,4,1,1,1);
}

QWidget* ValidationGroup::addSelectedSegmentButtons()
{
    QGroupBox* box = new QGroupBox("Set Selected Segments...", this);
    QGridLayout* layout = new QGridLayout(box);

    layout->addWidget(setSelectionValid,   0,0,1,1);
    layout->addWidget(setSelectionNotValid,1,0,1,1);
    layout->addWidget(setSelectionUncertain,   0,1,1,1);
    layout->addWidget(setSelectionNotUncertain,1,1,1,1);

    return box;
}

void ValidationGroup::changeMapColors()
{
    getViewGroupState()->SetShowValidMode(showValidatedButton->isChecked(),
                                          showValid->isChecked());
}

QString ValidationGroup::getGroupNameFromGUI()
{
    return mGroupName->text();
}

bool ValidationGroup::isShowValidChecked()
{
    return showValid->isChecked();
}

SegmentationDataWrapper ValidationGroup::GetSDW()
{
    return mDendToolBar->GetSDW();
}

OmViewGroupState * ValidationGroup::getViewGroupState()
{
    return mDendToolBar->getViewGroupState();
}
