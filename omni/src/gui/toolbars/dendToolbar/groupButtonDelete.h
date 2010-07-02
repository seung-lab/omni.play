#ifndef GROUP_BUTTON_DELETE_H
#define GROUP_BUTTON_DELETE_H

#include "gui/widgets/omButton.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"

class GroupButtonDelete : public OmButton<DendToolBar> {
 public:
	GroupButtonDelete(DendToolBar *);

 private:
	void doAction();
};

#endif
