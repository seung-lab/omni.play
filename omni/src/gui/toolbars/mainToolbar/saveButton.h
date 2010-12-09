#ifndef SAVE_BUTTON_H
#define SAVE_BUTTON_H

#include "gui/widgets/omButton.hpp"
#include "gui/toolbars/mainToolbar/mainToolbar.h"

class SaveButton : public OmButton<MainToolbar> {
 public:
	SaveButton(MainToolbar * mw);
 private:
	void doAction();
};

#endif
