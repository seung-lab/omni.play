#ifndef CUT_BUTTON_H
#define CUT_BUTTON_H

#include "gui/widgets/omButton.h"

class GraphTools;

class CutButton : public OmButton<GraphTools> {
 public:
	CutButton(GraphTools *);

 private:
	void doAction();
};

#endif
