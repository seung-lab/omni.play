#ifndef GROUP_BUTTON_DELETE_H
#define GROUP_BUTTON_DELETE_H

#include "gui/widgets/omButton.h"
#include "gui/toolbars/dendToolbar/validationGroup.h"

class GroupButtonDelete : public OmButton<ValidationGroup> {
 public:
	GroupButtonDelete(ValidationGroup *);

 private:
	void doAction();
};

#endif
