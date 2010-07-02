#ifndef SHOW_VALIDATED_BUTTON_H
#define SHOW_VALIDATED_BUTTON_H

#include "gui/widgets/omButton.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"

class ShowValidatedButton : public OmButton<DendToolBar> {
 public:
	ShowValidatedButton(DendToolBar *);

 private:
	void doAction();
};

#endif
