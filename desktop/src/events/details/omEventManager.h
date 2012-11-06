#pragma once

#include "events/details/omEvent.h"
#include "project/omProject.h"
#include "project/omProjectGlobals.h"

class OmEventListener;
class OmEventManagerImpl;

class OmEventManager : om::singletonBase<OmEventManager> {
private:
    inline static OmEventManagerImpl& impl(){
        return OmProject::Globals().EventManImpl();
    }

public:
    static void AddListener(OmEventClass eventClass,
                            OmEventListener* listener);

    static void RemoveListener(OmEventClass eventClass,
                               OmEventListener* listener);

    static void PostEvent(OmEvent* event);

private:
    OmEventManager();
    ~OmEventManager();

    friend class zi::singleton<OmEventManager>;
};

