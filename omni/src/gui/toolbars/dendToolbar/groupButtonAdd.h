#ifndef GROUP_BUTTON_ADD_H
#define GROUP_BUTTON_ADD_H

#include "gui/widgets/omButton.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"

class GroupButtonAdd : public OmButton<DendToolBar> {
 public:
	GroupButtonAdd(DendToolBar *);

 private:
	void doAction();
};

#endif
