#include "system/events/omEventManager.h"
#include "system/events/omViewEvent.h"
#include "system/events/omSegmentEvent.h"
#include "common/omDebug.h"
#include "zi/omUtility.h"

#include <qapplication.h>

OmEventManager *OmEventManager::mspInstance = 0;

OmEventManager* OmEventManager::Instance()
{
	if (NULL == mspInstance) {
		mspInstance = new OmEventManager;
	}
	return mspInstance;
}

void OmEventManager::Delete()
{
	if (mspInstance){
		delete mspInstance;
	}
	mspInstance = NULL;
}

bool OmEventManager::event(QEvent * event)
{
	//ensure we have an OmEvent object (or else the next step will pretty much crash)
	QEvent::Type eventType = event->type();
	ZI_VERIFY(eventType > QEvent::User && eventType < QEvent::MaxUser);

	//cast to OmEvent to get OmEventClass
	OmEvent *om_event = static_cast<OmEvent*>(event);

//	printf("OmEventManager::event: %s\n", om_event->EventTypeStr().c_str());

	OmEventClass event_class = om_event->ChildClass;

	//get set of listeners registered to an event class
	EventListenerSet & listener_set = mEventClassToListernsMap[event_class];

	//call event for every listener in the set
	FOR_EACH(it, listener_set){
		om_event->Dispatch(*it);
	}

	return true;
}

/////////////////////////////////
///////         EventListener Accessors

void OmEventManager::AddEventListener(OmEventClass eventClass,
									  OmEventListener* listener)
{
	mEventClassToListernsMap[eventClass].insert(listener);
}

void OmEventManager::RemoveEventListener(OmEventClass eventClass,
										 OmEventListener* listener)
{
	mEventClassToListernsMap[eventClass].erase(listener);
}

//macro to add listener to singleton
void OmEventManager::AddListener(OmEventClass eventClass,
								 OmEventListener* listener)
{
	Instance()->AddEventListener(eventClass, listener);
}

void OmEventManager::RemoveListener(OmEventClass eventClass,
									OmEventListener* listener)
{
	Instance()->RemoveEventListener(eventClass, listener);
}

/////////////////////////////////
///////         Event Processing
///////
///////         Macros to make use of Qt event handeling.

/*
 *	Event sent immediately to EventManager.  Event not deleted (normally created on stack).
 */
bool OmEventManager::SendEvent(OmEvent & event)
{
	return QCoreApplication::sendEvent(Instance(), &event);
}

/*
 *	Event put in queue.  Ownership of event object given to Qt, and deleted when
 *	notification completed (event normally created on heap).
 */
void OmEventManager::PostEvent(OmEvent* event)
{
	QCoreApplication::postEvent(Instance(), event);
}

void OmEventManager::FlushPostEvents()
{
	QCoreApplication::sendPostedEvents();
}
