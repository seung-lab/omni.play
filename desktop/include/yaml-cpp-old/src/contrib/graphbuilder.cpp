#include "yaml-cpp-old/parser.h"
#include "yaml-cpp-old/contrib/graphbuilder.h"
#include "graphbuilderadapter.h"

namespace YAMLold
{
  void *BuildGraphOfNextDocument(Parser& parser, GraphBuilderInterface& graphBuilder)
  {
    GraphBuilderAdapter eventHandler(graphBuilder);
    if (parser.HandleNextDocument(eventHandler)) {
      return eventHandler.RootNode();
    } else {
      return NULL;
    }
  }
}
