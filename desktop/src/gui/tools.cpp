#pragma once
#include "precomp.h"
#include "gui/tools.hpp"
std::string om::tool::ToolToString(om::tool::mode tool) {
  switch(tool) {
    case SELECT:
      return "SELECT";
    case PAN:
      return "PAN";
    case PAINT:
      return "PAINT";
    case ERASE:
      return "ERASE";
    case FILL:
      return "FILL";
    case SPLIT:
      return "SPLIT";
    case JOIN:
      return "JOIN";
    case CUT:
      return "CUT";
    case LANDMARK:
      return "LANDMARK";
    case ANNOTATE:
      return "ANNOTATE";
    case ADVANCED:
      return "ADVANCED";
    case SHATTER:
      return "SHATTER";
    case VALIDATE:
      return "VALIDATE";
    default:
      return "OTHER";
  }
}
