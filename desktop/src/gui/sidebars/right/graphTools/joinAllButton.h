#pragma once
#include "precomp.h"

#include "gui/widgets/omButton.hpp"
class GraphTools;

class JoinAllButton : public OmButton<GraphTools> {
  Q_OBJECT;

 public:
  JoinAllButton(GraphTools *);

 private:
  void doAction();
};
