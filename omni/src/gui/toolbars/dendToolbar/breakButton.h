#ifndef BREAK_BUTTON_H
#define BREAK_BUTTON_H

#include "gui/widgets/omButton.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"

class BreakButton : public OmButton<DendToolBar> {
 public:
	BreakButton(DendToolBar *);

 private:
	void doAction();
};

#endif
