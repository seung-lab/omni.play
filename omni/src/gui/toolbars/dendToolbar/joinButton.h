#ifndef JOIN_BUTTON_H
#define JOIn_BUTTON_H

#include "gui/widgets/omButton.h"
class GraphTools;

class JoinButton : public OmButton<GraphTools> {
 public:
	JoinButton(GraphTools *);

 private:
	void doAction();
};

#endif
