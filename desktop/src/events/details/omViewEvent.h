#pragma once

/*
 *
 * Brett Warne - bwarne@mit.edu - 3/14/09
 */

#include "events/details/omEvent.h"
#include "common/omStd.h"

/*
 * View Event
 */
class OmViewEvent : public OmEvent {

public:
    explicit OmViewEvent(QEvent::Type type);
    void Dispatch(OmEventListener *);

    //class
    static const OmEventClass CLASS = OM_VIEW_EVENT_CLASS;
    //events
    static const QEvent::Type VIEW_BOX_CHANGE = (QEvent::Type) (CLASS);
    static const QEvent::Type VIEW_CENTER_CHANGE = (QEvent::Type) (CLASS + 1);
    static const QEvent::Type VIEW_POS_CHANGE = (QEvent::Type) (CLASS + 2);

    static const QEvent::Type REDRAW = (QEvent::Type) (CLASS + 3);
    static const QEvent::Type REDRAW_BLOCKING = (QEvent::Type) (CLASS + 4);

    static const QEvent::Type COORD_SYSTEM_CHANGE = (QEvent::Type) (CLASS + 5);
};

/*
 *  View Event Listener
 */
class OmViewEventListener : public OmEventListener {

public:
    OmViewEventListener()
        : OmEventListener(OmViewEvent::CLASS)
    {};

    virtual void ViewBoxChangeEvent() {}
    virtual void ViewCenterChangeEvent() {}
    virtual void ViewPosChangeEvent() {}
    virtual void ViewRedrawEvent() {}
    virtual void ViewBlockingRedrawEvent() {}
    virtual void CoordSystemChangeEvent() {}
};

