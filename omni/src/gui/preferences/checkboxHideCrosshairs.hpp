#ifndef SHOW_CROSSHAIRS_CHECKBOX_HPP
#define SHOW_CROSSHAIRS_CHECKBOX_HPP

#include "view2d/om2dPreferences.hpp"
#include "gui/widgets/omCheckBoxWidget.hpp"
#include "system/omEvents.h"

class ShowCrosshairsCheckbox : public OmCheckBoxWidget {
public:
	ShowCrosshairsCheckbox(QWidget* p)
		: OmCheckBoxWidget(p, "Show crosshairs")
	{
		set(Om2dPreferences::ShowCrosshairs());
	}

private:
	void doAction(const bool isChecked)
	{
		Om2dPreferences::ShowCrosshairs(isChecked);
		OmEvents::Redraw2d();
	}
};

#endif
