
#include "omEventManager.h"
#include "events/omViewEvent.h"
#include "events/omSegmentEvent.h"

#include <qapplication.h>

#define DEBUG 0


//init instance pointer
OmEventManager* OmEventManager::mspInstance = 0;


//constructor
OmEventManager::OmEventManager() {

}


//singleton accessor
OmEventManager* OmEventManager::Instance() {
	
	if(NULL == mspInstance) {
		mspInstance = new OmEventManager;
	}
	
	return mspInstance;
}


void 
OmEventManager::Delete() {
	if(mspInstance) delete mspInstance;
	mspInstance = NULL;
}


bool 
OmEventManager::event(QEvent *event) {

	DOUT("OmEventManager::event(QEvent *event)");
	
	//ensure we have an OmEvent object (or else the next step will pretty much crash)
	QEvent::Type eventType = event->type();
	assert(eventType > QEvent::User && eventType < QEvent::MaxUser);

	//cast to OmEvent to get OmEventClass
	OmEvent *om_event = static_cast<OmEvent*>(event);
	OmEventClass event_class = om_event->ChildClass;
	
	//get set of listeners registered to an event class
	EventListenerSet &listener_set = mEventClassToListernsMap[event_class];
	
	//call event for every listener in the set
	EventListenerSet::iterator it;
	for ( it=listener_set.begin() ; it != listener_set.end(); it++ ) {
		
		//get listener
		OmEventListener *listener = *it;
		om_event->Dispatch(*it);
		
	}
	
	return true;
}



/////////////////////////////////
///////		EventListener Accessors

void 
OmEventManager::AddEventListener(OmEventClass eventClass, OmEventListener* listener) {
	mEventClassToListernsMap[eventClass].insert(listener);
}


void 
OmEventManager::RemoveEventListener(OmEventClass eventClass, OmEventListener* listener) {
	mEventClassToListernsMap[eventClass].erase(listener);
}

//macro to add listener to singleton
void 
OmEventManager::AddListener(OmEventClass eventClass, OmEventListener* listener) {
	Instance()->AddEventListener(eventClass, listener);
}


void 
OmEventManager::RemoveListener(OmEventClass eventClass, OmEventListener* listener) {
	Instance()->RemoveEventListener(eventClass, listener);
}




/////////////////////////////////
///////		Event Processing
///////
///////		Macros to make use of Qt event handeling.


/*
 *	Event sent immediately to EventManager.  Event not deleted (normally created on stack).
 */
bool 
OmEventManager::SendEvent(OmEvent &event) {
	bool status = QCoreApplication::sendEvent(Instance(), &event);
	//bool status = Instance()->event(&event);
	return status;
}


/*
 *	Event put in queue.  Ownership of event object given to Qt, and deleted when 
 *	notification completed (event normally created on heap).
 */
void 
OmEventManager::PostEvent(OmEvent* event) {
	DOUT("OmEventManager::PostEvent(OmEvent* event): " << event->ChildClass << " : " << event->type() );
	QCoreApplication::postEvent(Instance(), event);
}


void
OmEventManager::FlushPostEvents() {
	QCoreApplication::sendPostedEvents();
}
