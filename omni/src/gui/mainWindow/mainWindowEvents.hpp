#pragma once

#include "events/details/omNonFatalEvent.h"
#include "gui/widgets/omTellInfo.hpp"

class MainWindowEvents : public OmNonFatalEventListener {
public:
    void NonFatalEvent(OmNonFatalEvent* err){
        OmTellInfo info(err->Error());
    }
};
