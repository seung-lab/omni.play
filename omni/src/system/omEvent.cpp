
#include "omEvent.h"
#include "omEventManager.h"
#include "common/omDebug.h"

#define DEBUG 0

/////////////////////////////////
///////
///////          Example Class
///////

OmEventListener::OmEventListener(OmEventClass eventClass)
{
	//debug("FIXME", << "OmEventListener::OmEventListener()" << this << endl;
	//store the event class
	mEventClass = eventClass;
	OmEventManager::AddListener(eventClass, this);
}

OmEventListener::~OmEventListener()
{

	OmEventManager::RemoveListener(mEventClass, this);
}
