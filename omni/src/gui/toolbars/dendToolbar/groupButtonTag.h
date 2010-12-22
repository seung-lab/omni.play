#ifndef GROUP_BUTTON_TAG_H
#define GROUP_BUTTON_TAG_H

#include "gui/widgets/omButton.hpp"
#include "gui/toolbars/dendToolbar/validationGroup.h"

class GroupButtonTag : public OmButton<ValidationGroup> {
 public:
	GroupButtonTag(ValidationGroup *);

 private:
	void doAction();
};

#endif
