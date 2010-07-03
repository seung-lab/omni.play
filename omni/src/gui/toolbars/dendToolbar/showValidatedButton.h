#ifndef SHOW_VALIDATED_BUTTON_H
#define SHOW_VALIDATED_BUTTON_H

#include "gui/widgets/omButton.h"
#include "gui/toolbars/dendToolbar/validationGroup.h"

class ShowValidatedButton : public OmButton<ValidationGroup> {
 public:
	ShowValidatedButton(ValidationGroup *);

 private:
	void doAction();
};

#endif
