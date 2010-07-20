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
