#ifndef SAVE_BUTTON_H
#define SAVE_BUTTON_H

#include "gui/widgets/omButton.hpp"
#include "gui/toolbars/mainToolbar/mainToolbar.h"

class SaveButton : public OmButton<MainToolBar> {
public:
    SaveButton(MainToolBar * mw);
private:
    void doAction();
};

#endif
