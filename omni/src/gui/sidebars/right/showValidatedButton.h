#pragma once

#include "gui/widgets/omButton.hpp"
#include "gui/sidebars/right/validationGroup.h"

class ShowValidatedButton : public OmButton<ValidationGroup> {
 public:
	ShowValidatedButton(ValidationGroup *);

 private:
	void doAction();
};

