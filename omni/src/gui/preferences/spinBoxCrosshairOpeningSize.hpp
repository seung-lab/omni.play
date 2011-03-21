#ifndef CROSSHAIR_OPENING_SIZE_SPIN_BOX_HPP
#define CROSSHAIR_OPENING_SIZE_SPIN_BOX_HPP

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
	void actUponSpinboxChange(const int val)
	{
		Om2dPreferences::CrosshairHoleSize(val);
		OmEvents::Redraw2d();
	}
};

#endif
