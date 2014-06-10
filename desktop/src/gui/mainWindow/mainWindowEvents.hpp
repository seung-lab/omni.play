#pragma once
#include "precomp.h"

#include "events/details/nonFatalEvent.h"
#include "events/listeners.h"
#include "events/details/executeOnMainEvent.h"
#include "gui/widgets/omTellInfo.hpp"

class MainWindowEvents : public om::event::NonFatalEventListener,
                         public om::event::ExecuteOnMainEventListener {
 public:
  void NonFatalEvent(om::event::NonFatalEvent* err) {
    OmTellInfo logInfos(err->Error());
  }

  void ExecuteOnMainEvent(om::event::ExecuteOnMainEvent* evt) {
    if (evt && evt->func()) {
      evt->func()();
    }
  }
};
