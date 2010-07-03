#ifndef SPLIT_BUTTON_H
#define SPLIT_BUTTON_H

#include "gui/widgets/omButton.h"

class GraphTools;

class SplitButton : public OmButton<GraphTools> {
 public:
	SplitButton(GraphTools *);

 private:
	void doAction();
};

#endif
