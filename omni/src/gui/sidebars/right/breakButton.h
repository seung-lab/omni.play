#ifndef BREAK_BUTTON_H
#define BREAK_BUTTON_H

#include "gui/widgets/omButton.hpp"

class GraphTools;

class BreakButton : public OmButton<GraphTools> {
Q_OBJECT

public:
    BreakButton(GraphTools*);

private:
    void doAction();

private Q_SLOTS:
    void showOrHideBreakThresholdBox(const bool show);
};

#endif
