#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/graphTools.h"
#include "gui/widgets/thresholdGroup/omThresholdGroup.h"
#include "system/omEvents.h"

OmThresholdGroup::OmThresholdGroup(QWidget * d)
	: QDoubleSpinBox(d)
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
	OmEvents::Redraw();
}

double OmThresholdGroup::getGUIvalue()
{
	return value();
}

void OmThresholdGroup::setGUIvalue(const double newThreshold)
{
	setValue(newThreshold);
}
