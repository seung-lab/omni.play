#pragma once

#include "view2d/om2dPreferences.hpp"
#include "gui/widgets/omCheckBoxWidget.hpp"
#include "events/omEvents.h"

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

