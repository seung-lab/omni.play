#ifndef JOIN_BUTTON_H
#define JOIN_BUTTON_H

#include "gui/widgets/omButton.hpp"
class GraphTools;

class JoinButton : public OmButton<GraphTools> {
public:
    JoinButton(GraphTools *);

private:
    void doAction();
};

#endif
