#ifndef SPLIT_BUTTON_H
#define SPLIT_BUTTON_H

#include "gui/widgets/omButton.hpp"

class GraphTools;

class SplitButton : public OmButton<GraphTools> {
Q_OBJECT

public:
    SplitButton(GraphTools *);

private:
    void doAction()
    {}

private Q_SLOTS:
    void enterOrExitSplitMode(const bool show);
};

#endif
