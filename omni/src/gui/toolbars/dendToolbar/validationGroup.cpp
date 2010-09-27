#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/validationGroup.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
#include "utility/dataWrappers.h"
#include "gui/toolbars/dendToolbar/groupButtonAdd.h"
#include "gui/toolbars/dendToolbar/groupButtonDelete.h"
#include "gui/toolbars/dendToolbar/groupButtonTag.h"
#include "gui/toolbars/dendToolbar/showValidatedButton.h"
#include "system/viewGroup/omViewGroupState.h"

ValidationGroup::ValidationGroup(DendToolBar * d)
	: OmWidget(d)
	, mDendToolBar(d)
	, groupButtonAdd(new GroupButtonAdd(this))
	, groupButtonDelete(new GroupButtonDelete(this))
	, groupButtonTag(new GroupButtonTag(this))
	, showValidatedButton(new ShowValidatedButton(this))
{
	validGroup = new QButtonGroup();
	showValid = new QRadioButton("In Color");
	validGroup->addButton(showValid);
        connect(showValid, SIGNAL(toggled(bool)),
                this, SLOT(changeMapColors()));

	dontShowValid = new QRadioButton("As Black");
	dontShowValid->setChecked(true);
	validGroup->addButton(dontShowValid);
        connect(dontShowValid, SIGNAL(toggled(bool)),
                this, SLOT(changeMapColors()));

        mGroupName = new QLineEdit(this);
        mGroupName->setText("Glia");

        QGridLayout * box = new QGridLayout(this);
        box->addWidget(groupButtonAdd,0,0,1,2);
	box->addWidget(groupButtonDelete,1,0,1,2);
        box->addWidget(showValidatedButton,2,0,1,2);
        box->addWidget(showValid,3,0,1,1);
        box->addWidget(dontShowValid,3,1,1,1);
        box->addWidget(groupButtonTag,4,0,1,1);
        box->addWidget(mGroupName,4,1,1,1);
}

void ValidationGroup::changeMapColors()
{
	//debug(valid, "ValidationGroup::changeMapColors(%i)\n", showValidatedButton->isChecked());
	// Using !(not) because check happens after this fuction.
	getViewGroupState()->SetShowValidMode(showValidatedButton->isChecked(), showValid->isChecked());
}

QString ValidationGroup::getGroupNameFromGUI()
{
	return mGroupName->text();
}

bool ValidationGroup::isShowValidChecked()
{
	return showValid->isChecked();
}

SegmentationDataWrapper ValidationGroup::getSegmentationDataWrapper()
{
	return mDendToolBar->getSegmentationDataWrapper();
}

OmViewGroupState * ValidationGroup::getViewGroupState()
{
	return mDendToolBar->getViewGroupState();
}
