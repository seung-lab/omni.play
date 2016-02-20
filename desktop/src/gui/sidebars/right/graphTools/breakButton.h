#pragma once
#include "precomp.h"

#include "gui/widgets/omButton.hpp"

class GraphTools;

class BreakButton : public OmButton<GraphTools> {
  Q_OBJECT;

 public:
  BreakButton(GraphTools*);

 private:
  void doAction() {}

 private
Q_SLOTS:
  void SetShouldVolumeBeShownBroken(const bool shouldVolumeBeShownBroken);
};
