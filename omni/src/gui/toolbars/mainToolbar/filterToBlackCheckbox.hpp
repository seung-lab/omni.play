#ifndef FILTER_TO_BLACK_CHECKBOX_HPP
#define FILTER_TO_BLACK_CHECKBOX_HPP

#include "view2d/om2dPreferences.hpp"
#include "gui/widgets/omCheckBoxWidget.hpp"

class FilterToBlackCheckbox : public OmCheckBoxWidget {
public:
	FilterToBlackCheckbox(QWidget* p)
		: OmCheckBoxWidget(p, "Fade To Black")
	{
		set(Om2dPreferences::HaveAlphaGoToBlack());
	}

private:
	void doAction(const bool isChecked)
	{
		Om2dPreferences::HaveAlphaGoToBlack(isChecked);
		OmEvents::Redraw2d();
	}
};

#endif
