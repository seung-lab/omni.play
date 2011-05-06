#ifndef CUT_BUTTON_H
#define CUT_BUTTON_H

#include "gui/widgets/omButton.hpp"

class GraphTools;

class CutButton : public OmButton<GraphTools> {
Q_OBJECT

public:
    CutButton(GraphTools *);

private:
    void doAction()
    {}

private Q_SLOTS:
    void enableCutMode(const bool show);
};

#endif
