#pragma once

#include "view2d/om2dPreferences.hpp"
#include "gui/widgets/omIntSpinBox.hpp"
#include "events/omEvents.h"

class CrosshairOpeningSizeSpinBox : public OmIntSpinBox {
public:
	CrosshairOpeningSizeSpinBox(QWidget* p)
		: OmIntSpinBox(p, om::UPDATE_AS_TYPE)
	{
		setSingleStep(1);
		setMinimum(0);
		setMaximum(100);
		setValue(Om2dPreferences::CrosshairHoleSize());
	}

private:
	void actUponValueChange(const int val)
	{
		Om2dPreferences::CrosshairHoleSize(val);
		OmEvents::Redraw2d();
	}
};

