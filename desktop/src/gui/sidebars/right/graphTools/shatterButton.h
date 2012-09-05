#pragma once

#include "gui/widgets/omButton.hpp"

class GraphTools;

class ShatterButton : public OmButton<GraphTools> {
Q_OBJECT

public:
    ShatterButton(GraphTools*);

private:
    void doAction()
    {}

private Q_SLOTS:
    void enterOrExitShatterMode(const bool show);
};
