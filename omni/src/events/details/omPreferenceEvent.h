#pragma once

/*
 *
 *	Brett Warne - bwarne@mit.edu - 3/14/09
 */


#include <common/omStd.h>

#include "events/details/omEvent.h"

class OmPreferenceEvent : public OmEvent {

public:
    OmPreferenceEvent(QEvent::Type type, int preference);
    void Dispatch(OmEventListener *);

    //class
    static const OmEventClass CLASS  = OM_PREFERENCE_EVENT_CLASS;

    //events
    static const QEvent::Type PREFERENCE_CHANGE = (QEvent::Type) (CLASS);

    int GetPreference() { return mPreference; }

private:
    int mPreference;
};

class OmPreferenceEventListener : public OmEventListener {

public:
    OmPreferenceEventListener() : OmEventListener(OmPreferenceEvent::CLASS) { };

    virtual void PreferenceChangeEvent(OmPreferenceEvent * event) = 0;
};



