#ifndef AUTO_BREAK_CHECKBOX_H
#define AUTO_BREAK_CHECKBOX_H

#include "gui/widgets/omCheckbox.h"

class GraphTools;

class AutoBreakCheckbox : public OmCheckBox<GraphTools> {
 public:
	AutoBreakCheckbox(GraphTools*);

 private:
	void doAction();
};

#endif
