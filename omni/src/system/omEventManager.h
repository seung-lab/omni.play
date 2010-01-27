#ifndef OM_EVENT_MANAGER_H
#define OM_EVENT_MANAGER_H

/*
 *	OmEventManager is a hybrid event notification system using Qt's event system to
 *	dispatch events to sets of listeners that have registered with the manager.
 *
 *	Brett Warne - bwarne@mit.edu - 2/11/09
 */

#include <common/omStd.h>

#include <QObject>
#include <QEvent>

#include "omEvent.h"

typedef set< OmEventListener* > EventListenerSet;
typedef map< OmEventClass, EventListenerSet > EventClassToEventListenerSetMap;


class OmEventManager : public QObject {

public:
	static OmEventManager* Instance();
	static void Delete();
	
	//qtevent handler
	bool event(QEvent *event);	//customEvent
	
	//EventListener accessors
	void AddEventListener(OmEventClass, OmEventListener*);
	void RemoveEventListener(OmEventClass, OmEventListener*);
	//static accessors
	static void AddListener(OmEventClass, OmEventListener*);
	static void RemoveListener(OmEventClass, OmEventListener*);
	
	//event accessors
	static bool SendEvent(OmEvent&);
	static void PostEvent(OmEvent*);
	static void FlushPostEvents();
	
protected:
	// singleton constructor, copy constructor, assignment operator protected
	OmEventManager();
	OmEventManager(const OmEventManager&);
	OmEventManager& operator= (const OmEventManager&);

private:
	//singleton
	static OmEventManager* mspInstance;
	
	//map events
	EventClassToEventListenerSetMap mEventClassToListernsMap;
};



#endif
