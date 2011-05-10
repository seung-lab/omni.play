#pragma once

/*
 *
 *  Brett Warne - bwarne@mit.edu - 4/6/09
 */

#include "common/omStd.h"
#include "common/omException.h"

#include <QObject>
#include <QEvent>

enum OmEventClass { OM_VIEW_EVENT_CLASS = 2000,
                    OM_SEGMENT_EVENT_CLASS = 3000,
                    OM_VIEW_3D_EVENT_CLASS = 4000,
                    OM_USER_INTERFACE_EVENT_CLASS = 5000,
                    OM_TOOL_MODE_EVENT_CLASS = 6000,
                    OM_PREFERENCE_EVENT_CLASS = 8000};

class OmEventListener;

class OmEvent : public QEvent {
public:
    virtual void Dispatch(OmEventListener *) = 0;

    const OmEventClass ChildClass;

    std::string EventTypeStr() const
    {
        switch(ChildClass){
        case 2000:
            return "OM_VIEW_EVENT_CLASS";
        case 3000:
            return "OM_SEGMENT_EVENT_CLASS";
        case 4000:
            return "OM_VIEW_3D_EVENT_CLASS";
        case 6000:
            return "OM_TOOL_MODE_EVENT_CLASS";
        case 7000:
            return "OM_PROGRESS_EVENT_CLASS";
        case 8000:
            return "OM_PREFERENCE_EVENT_CLASS";
        default:
            throw OmArgException("unknown event class");
        }
    }

protected:
    OmEvent(QEvent::Type type, OmEventClass eventClass)
        : QEvent(type)
        , ChildClass(eventClass)
    {}
};

class OmEventListener {
protected:
    OmEventListener(OmEventClass eventClass);

    //no unique final overridder
    //virtual bool Event(OmEvent *event) = 0;

    //dynamic cast in event manager requires at least
    //one virtual function in the base class
    virtual ~OmEventListener();

private:
    OmEventListener()
    {}

    OmEventClass eventClass_;
};

