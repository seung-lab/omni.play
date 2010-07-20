#ifndef BREAK_BUTTON_H
#define BREAK_BUTTON_H

#include "gui/widgets/omButton.h"
class GraphTools;

class BreakButton : public OmButton<GraphTools> {
 public:
	BreakButton(GraphTools *);

 private:
	void doAction();
};

#endif
