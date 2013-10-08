#pragma once

#include "gui/widgets/omButton.hpp"
class GraphTools;

class JoinButton : public OmButton<GraphTools> {
 public:
  JoinButton(GraphTools *);

 private:
  void doAction();
};
