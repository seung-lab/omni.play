#include "gui/toolbars/dendToolbar/displayTools.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
#include "system/omEvents.h"
#include "system/viewGroup/omViewGroupState.h"
#include "utility/dataWrappers.h"
#include "gui/toolbars/dendToolbar/dust3DthresholdGroup.h"

DisplayTools::DisplayTools(DendToolBar * d)
	: QGroupBox("Display Tools", d)
	, mParent(d)
{
	QVBoxLayout* box = new QVBoxLayout(this);
	box->addWidget(thresholdBox());
	box->addWidget(filterShowNonSelectedSegmentsBox());
}

void DisplayTools::changeMapColors()
{
	// Using !(not) because check happens after this fuction.
	getViewGroupState()->SetShowFilterMode(showValid->isChecked());
}

QWidget* DisplayTools::filterShowNonSelectedSegmentsBox()
{
	QGroupBox* widget = new QGroupBox("Show Non-selected Segments", this);

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

	QGridLayout * box = new QGridLayout(widget);
        box->addWidget(showValid,3,0,1,1);
        box->addWidget(dontShowValid,3,1,1,1);

	return widget;
}

QWidget* DisplayTools::thresholdBox()
{
	QGroupBox* widget = new QGroupBox("3D Dust Threshold", this);
	Dust3DThresholdGroup* thresholdBox = new Dust3DThresholdGroup(this);

	QHBoxLayout* layout = new QHBoxLayout(widget);
	layout->addWidget(thresholdBox);

	return widget;
}

OmViewGroupState * DisplayTools::getViewGroupState()
{
	return mParent->getViewGroupState();
}

void DisplayTools::updateGui()
{
	OmEvents::Redraw();
}

SegmentationDataWrapper DisplayTools::getSegmentationDataWrapper()
{
	return mParent->getSegmentationDataWrapper();
}
