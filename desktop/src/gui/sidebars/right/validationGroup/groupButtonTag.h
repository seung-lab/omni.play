#pragma once

#include "gui/widgets/omButton.hpp"
#include "gui/sidebars/right/validationGroup/validationGroup.h"

class GroupButtonTag : public OmButton<ValidationGroup> {
 public:
  GroupButtonTag(ValidationGroup *);

 private:
  void doAction();
};
