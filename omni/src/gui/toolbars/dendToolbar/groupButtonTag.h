#ifndef GROUP_BUTTON_TAG_H
#define GROUP_BUTTON_TAG_H

#include "gui/widgets/omButton.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"

class GroupButtonTag : public OmButton<DendToolBar> {
 public:
	GroupButtonTag(DendToolBar *);

 private:
	void doAction();
};

#endif
