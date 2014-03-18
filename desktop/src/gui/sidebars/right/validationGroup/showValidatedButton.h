#pragma once
#include "precomp.h"

#include "gui/widgets/omButton.hpp"
#include "gui/sidebars/right/validationGroup/validationGroup.h"

class ShowValidatedButton : public OmButton<ValidationGroup> {
 public:
  ShowValidatedButton(ValidationGroup *);

 private:
  void doAction();
};
