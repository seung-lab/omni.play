#ifndef CUT_BUTTON_H
#define CUT_BUTTON_H

#include "gui/widgets/omButton.hpp"

class GraphTools;

class CutButton : public OmButton<GraphTools> {
 public:
	CutButton(GraphTools *);

 private:
	void doAction();
};

#endif
