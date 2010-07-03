#ifndef AUTO_BREAK_CHECKBOX_H
#define AUTO_BREAK_CHECKBOX_H

#include "gui/widgets/omCheckbox.h"

class DendToolBar;

class AutoBreakCheckbox : public OmCheckBox<DendToolBar> {
 public:
	AutoBreakCheckbox(DendToolBar*);

 private:
	void doAction();
};

#endif
