#pragma once

#include "events/details/omEvent.h"
#include "common/omStd.h"

class OmRefreshMSTthresholdEvent : public OmEvent {

public:
    OmRefreshMSTthresholdEvent();

    void Dispatch(OmEventListener*);

    //class
    static const OmEventClass CLASS = OM_REFRESH_MST_THRESHOLD_EVENT_CLASS;

    //events
    static const QEvent::Type REFRESH_MST_THRESHOLD = (QEvent::Type) (CLASS);
};

class OmRefreshMSTthresholdEventListener : public OmEventListener {

public:
    OmRefreshMSTthresholdEventListener()
        : OmEventListener(OmRefreshMSTthresholdEvent::CLASS)
    {};

    virtual void RefreshMSTEvent(OmRefreshMSTthresholdEvent*)
    {}
};

