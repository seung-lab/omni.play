#pragma once

#include "gui/widgets/omButton.hpp"

class GraphTools;

class SplitButton : public OmButton<GraphTools> {
  Q_OBJECT public : SplitButton(GraphTools *);

 private:
  void doAction() {}

 private
Q_SLOTS:
  void enterOrExitSplitMode(const bool show);
};
