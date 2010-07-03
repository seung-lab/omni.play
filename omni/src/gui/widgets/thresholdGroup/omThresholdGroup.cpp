#include "common/omDebug.h"
#include "gui/widgets/thresholdGroup/omThresholdGroup.h"
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
	
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	actUponThresholdChange(getGUIvalue());
	updateGui();
	QApplication::restoreOverrideCursor();
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
