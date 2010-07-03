#ifndef GROUP_BUTTON_ADD_H
#define GROUP_BUTTON_ADD_H

#include "gui/widgets/omButton.h"
#include "gui/toolbars/dendToolbar/validationGroup.h"

class GroupButtonAdd : public OmButton<ValidationGroup> {
 public:
	GroupButtonAdd(ValidationGroup *);

 private:
	void doAction();
};

#endif
