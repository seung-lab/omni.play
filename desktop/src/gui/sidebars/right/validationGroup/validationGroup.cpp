#include "common/omDebug.h"
#include "gui/sidebars/right/rightImpl.h"
#include "gui/sidebars/right/validationGroup/groupButtonTag.h"
#include "gui/sidebars/right/validationGroup/setNotUncertain.hpp"
#include "gui/sidebars/right/validationGroup/setNotValid.hpp"
#include "gui/sidebars/right/validationGroup/setUncertain.hpp"
#include "gui/sidebars/right/validationGroup/setValid.hpp"
#include "gui/sidebars/right/validationGroup/showValidatedButton.h"
#include "gui/sidebars/right/validationGroup/validationGroup.h"
#include "system/omConnect.hpp"
#include "utility/dataWrappers.h"
#include "utility/segmentationDataWrapper.hpp"
#include "viewGroup/omViewGroupState.h"
#include "viewGroup/omViewGroupState.h"

ValidationGroup::ValidationGroup(om::sidebars::rightImpl* d, OmViewGroupState* vgs)
    : OmWidget(d)
    , vgs_(vgs)
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

    QFormLayout * form = new QFormLayout(this);
    QWidget * w = new QWidget(this);
    form->addWidget(w);

    QGridLayout * box = new QGridLayout(w);
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
    GetViewGroupState()->SetShowValidMode(showValidatedButton->isChecked(),
                                          showValid->isChecked());
}

QString ValidationGroup::getGroupNameFromGUI(){
    return mGroupName->text();
}

bool ValidationGroup::isShowValidChecked(){
    return showValid->isChecked();
}

OmViewGroupState* ValidationGroup::GetViewGroupState() const {
    return vgs_;
}

SegmentationDataWrapper ValidationGroup::GetSDW() const {
    return vgs_->Segmentation();
}
