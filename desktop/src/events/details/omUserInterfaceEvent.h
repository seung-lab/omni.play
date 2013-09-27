#pragma once

#include "events/details/omEvent.h"

/*
 * User Interface Event
 */
class OmUserInterfaceEvent : public OmEvent {

public:
    explicit OmUserInterfaceEvent(QEvent::Type type);
    OmUserInterfaceEvent(QWidget* widget, const QString& title);

    void Dispatch(OmEventListener *);

    //class
    static const OmEventClass CLASS = OM_USER_INTERFACE_EVENT_CLASS;

    //events
    static const QEvent::Type UPDATE_SEGMENT_PROP_WIDGET = (QEvent::Type) (CLASS);

    QWidget* Widget(){
        return widget_;
    }

    const QString& Title(){
        return title_;
    }

private:
    QWidget* widget_;
    QString title_;
};

/*
 *  User Interface Event Listener
 */
class OmUserInterfaceEventListener : public OmEventListener {

public:
    OmUserInterfaceEventListener()
        : OmEventListener(OmUserInterfaceEvent::CLASS)
    {};

    virtual void UpdateSegmentPropWidgetEvent(OmUserInterfaceEvent*)
    {}
};

