#pragma once

#include "events/details/nonFatalEvent.h"
#include "events/listeners.h"
#include "gui/widgets/omTellInfo.hpp"

class MainWindowEvents : public om::event::NonFatalEventListener {
 public:
  void NonFatalEvent(om::event::NonFatalEvent* err) {
    OmTellInfo logInfos(err->Error());
  }
};
