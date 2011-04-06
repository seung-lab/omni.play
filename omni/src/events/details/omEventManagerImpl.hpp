#ifndef OM_EVENT_MANAGER_IMPL_HPP
#define OM_EVENT_MANAGER_IMPL_HPP

/*
 * OmEventManager is a hybrid event notification system using Qt's event system to
 *   dispatch events to sets of listeners that have registered with the manager.
 *
 * Brett Warne - bwarne@mit.edu - 2/11/09
 */

#include "common/omCommon.h"
#include "events/details/omEvent.h"
#include "events/details/omEventManager.h"
#include "zi/omUtility.h"
#include "zi/omMutex.h"

#include <qapplication.h>
#include <QObject>
#include <QEvent>

class OmEventManagerImpl : public QObject {
private:
    typedef std::set<OmEventListener*> listeners_t;

    std::map<OmEventClass, listeners_t> classListeners_;

    zi::mutex lock_;

public:
    OmEventManagerImpl()
    {}

    ~OmEventManagerImpl()
    {}

    void AddListener(OmEventClass eventClass,
                     OmEventListener* listener)
    {
        zi::guard g(lock_);
        classListeners_[eventClass].insert(listener);
    }

    void RemoveListener(OmEventClass eventClass,
                        OmEventListener* listener)
    {
        zi::guard g(lock_);
        classListeners_[eventClass].erase(listener);
    }

    //event accessors
    inline void PostEvent(OmEvent* event){
        QCoreApplication::postEvent(this, event);
    }

private:

    //qtevent handler for custom event
    bool event(QEvent* event)
    {
        const QEvent::Type eventType = event->type();

        //ensure we have an OmEvent object
        ZI_VERIFY(eventType > QEvent::User && eventType < QEvent::MaxUser);

        OmEvent* omEvent = static_cast<OmEvent*>(event);

        const OmEventClass eventKlass = omEvent->ChildClass;

        {
            zi::guard g(lock_);

            if(!classListeners_.count(eventKlass)){
                return true;
            }

            listeners_t& listeners = classListeners_[eventKlass];

            FOR_EACH(it, listeners){
                omEvent->Dispatch(*it);
            }
        }

        return true;
    }
};

#endif
