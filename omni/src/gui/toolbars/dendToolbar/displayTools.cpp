#include "gui/toolbars/dendToolbar/sliceDepthSpinBoxX.hpp"
#include "gui/toolbars/dendToolbar/sliceDepthSpinBoxY.hpp"
#include "gui/toolbars/dendToolbar/sliceDepthSpinBoxZ.hpp"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
#include "gui/toolbars/dendToolbar/displayTools.h"
#include "gui/toolbars/dendToolbar/dust3DthresholdGroup.h"
#include "system/omEvents.h"
#include "utility/dataWrappers.h"
#include "viewGroup/omViewGroupState.h"

DisplayTools::DisplayTools(DendToolBar * d)
	: OmWidget(d)
	, mParent(d)
{
	QVBoxLayout* box = new QVBoxLayout(this);
	box->addWidget(thresholdBox());
	box->addWidget(filterShowNonSelectedSegmentsBox());
	box->addWidget(view2dSliceDepthBox());
}

QWidget* DisplayTools::view2dSliceDepthBox()
{
	QGroupBox* widget = new QGroupBox("Slice Depths (x,y,z)", this);

	SliceDepthSpinBoxX* xDepth = new SliceDepthSpinBoxX(this);
	SliceDepthSpinBoxY* yDepth = new SliceDepthSpinBoxY(this);
	SliceDepthSpinBoxZ* zDepth = new SliceDepthSpinBoxZ(this);

	QHBoxLayout* layout = new QHBoxLayout(widget);
	layout->addWidget(xDepth);
	layout->addWidget(yDepth);
	layout->addWidget(zDepth);

	return widget;
}

void DisplayTools::changeMapColorsSlot()
{
	const bool val = showValid->isChecked();

	getViewGroupState()->
		SetHowNonSelectedSegmentsAreColoredInFilter(val);
}

QWidget* DisplayTools::filterShowNonSelectedSegmentsBox()
{
	QGroupBox* widget = new QGroupBox("Show Non-selected Segments", this);

	validGroup = new QButtonGroup();
	showValid = new QRadioButton("In Color");
	validGroup->addButton(showValid);
	connect(showValid, SIGNAL(toggled(bool)),
			this, SLOT(changeMapColorsSlot()));

	dontShowValid = new QRadioButton("As Black");
	dontShowValid->setChecked(true);
	validGroup->addButton(dontShowValid);
	connect(dontShowValid, SIGNAL(toggled(bool)),
			this, SLOT(changeMapColorsSlot()));

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
	OmEvents::Redraw2d();
}

SegmentationDataWrapper DisplayTools::GetSegmentationDataWrapper()
{
	return mParent->GetSegmentationDataWrapper();
}
