
#include "omEvent.h"
#include "omEventManager.h"

#define DEBUG 0



#pragma mark -
#pragma mark Example Class
/////////////////////////////////
///////
///////		 Example Class
///////


OmEventListener::OmEventListener(OmEventClass eventClass) {
	//cout << "OmEventListener::OmEventListener()" << this << endl;
	//store the event class
	mEventClass = eventClass;
	OmEventManager::AddListener(eventClass, this);
}


OmEventListener::~OmEventListener() {

	OmEventManager::RemoveListener(mEventClass, this);
}