#ifndef OM_USER_INTERFACE_EVENT_CLASS_H
#define OM_USER_INTERFACE_EVENT_CLASS_H

#include "events/details/omEvent.h"
#include "common/omStd.h"

/*
 * User Interface Event
 */
class OmUserInterfaceEvent : public OmEvent {

public:
    OmUserInterfaceEvent(QEvent::Type type);
    OmUserInterfaceEvent(QWidget* widget, const QString& title);

    void Dispatch(OmEventListener *);

    //class
    static const OmEventClass CLASS = OM_USER_INTERFACE_EVENT_CLASS;

    //events
    static const QEvent::Type UPDATE_SEGMENT_PROP_WIDGET = (QEvent::Type) (CLASS);
    // static const QEvent::Type VIEW_CENTER_CHANGE = (QEvent::Type) (CLASS + 1);
    // static const QEvent::Type VIEW_POS_CHANGE = (QEvent::Type) (CLASS + 2);
    // static const QEvent::Type REDRAW = (QEvent::Type) (CLASS + 3);
    // static const QEvent::Type REDRAW_BLOCKING = (QEvent::Type) (CLASS + 4);

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

    // virtual void ViewCenterChangeEvent() = 0;
    // virtual void ViewPosChangeEvent() = 0;
    // virtual void ViewRedrawEvent() = 0;
    // virtual void ViewBlockingRedrawEvent() = 0;
};

#endif
