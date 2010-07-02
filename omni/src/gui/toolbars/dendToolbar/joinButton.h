#ifndef JOIN_BUTTON_H
#define JOIn_BUTTON_H

#include "gui/widgets/omButton.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"

class JoinButton : public OmButton<DendToolBar> {
 public:
	JoinButton(DendToolBar *);

 private:
	void doAction();
};

#endif
