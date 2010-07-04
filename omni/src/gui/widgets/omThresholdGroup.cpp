#include "common/omDebug.h"
#include "gui/widgets/omCursors.h"
#include "gui/widgets/omThresholdGroup.h"
#include "system/omEvents.h"

OmThresholdGroup::OmThresholdGroup(QWidget * d, const bool updateAsType)
	: QDoubleSpinBox(d)
{
	if(updateAsType){
		connect(this, SIGNAL(valueChanged(double)), 
			this, SLOT(thresholdChanged()));
	} else {
	connect(this, SIGNAL(editingFinished()), 
		this, SLOT(thresholdChanged()));
	}
}

void OmThresholdGroup::thresholdChanged()
{
	debug("dendbar", "OmThresholdGroup::thresholdChanged\n");

	OmBusyCursorWrapper busyCursorWrapper();

	actUponThresholdChange(getGUIvalue());
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
