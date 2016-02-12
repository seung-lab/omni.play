#pragma once
#include "precomp.h"

#include "gui/widgets/omButton.hpp"
class GraphTools;

class JoinButton : public OmButton<GraphTools> {
 public:
  JoinButton(GraphTools *);

 private:
  void doAction();

 private
Q_SLOTS:
  void enterOrExitJoinMode(const bool show);
};
