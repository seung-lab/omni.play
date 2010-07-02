#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
#include "gui/widgets/thresholdGroup/omThresholdGroup.h"

OmThresholdGroup::OmThresholdGroup(DendToolBar * d, const QString & boxName)
	: QDoubleSpinBox(d)
	, mDendToolbar(d)
{
	connect(this, SIGNAL(valueChanged(double)), 
		this, SLOT(thresholdChanged(double)));
}

void OmThresholdGroup::thresholdChanged(const double valueFromGUI)
{
	debug("dendbar", "OmThresholdGroup::thresholdChanged\n");
	actUponThresholdChange(valueFromGUI);
	updateGui();
}

void OmThresholdGroup::updateGui()
{
	mDendToolbar->updateGui();
}

double OmThresholdGroup::getGUIvalue()
{
	return value();
}

void OmThresholdGroup::setGUIvalue(const double newThreshold)
{
	setValue(newThreshold);
}
