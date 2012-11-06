#pragma once

#include "common/omStd.h"
#include "common/exception.h"

#include <QObject>
#include <QEvent>

enum OmEventClass { OM_VIEW_EVENT_CLASS = 2000,
                    OM_SEGMENT_EVENT_CLASS = 3000,
                    OM_VIEW_3D_EVENT_CLASS = 4000,
                    OM_USER_INTERFACE_EVENT_CLASS = 5000,
                    OM_TOOL_MODE_EVENT_CLASS = 6000,
                    OM_REFRESH_MST_THRESHOLD_EVENT_CLASS = 7000,
                    OM_PREFERENCE_EVENT_CLASS = 8000,
                    OM_NON_FATAL_EVENT_CLASS = 9000,
                    OM_ANNOTATION_EVENT_CLASS = 10000
};

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
        case 5000:
            return "OM_USER_INTERFACE_EVENT_CLASS";
        case 6000:
            return "OM_TOOL_MODE_EVENT_CLASS";
        case 7000:
            return "OM_REFRESH_MST_THRESHOLD_EVENT_CLASS";
        case 8000:
            return "OM_PREFERENCE_EVENT_CLASS";
        case 9000:
            return "OM_NON_FATAL_EVENT_CLASS";
        default:
            throw om::ArgException("unknown event class");
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

