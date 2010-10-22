#ifndef OM_EVENT_MANAGER_H
#define OM_EVENT_MANAGER_H

/*
 *	OmEventManager is a hybrid event notification system using Qt's event system to
 *	dispatch events to sets of listeners that have registered with the manager.
 *
 *	Brett Warne - bwarne@mit.edu - 2/11/09
 */

#include "common/omCommon.h"
#include "system/events/omEvent.h"

#include <QObject>
#include <QEvent>

class OmEventListener;

typedef std::set<OmEventListener*> EventListenerSet;
typedef std::map<OmEventClass, EventListenerSet> EventClassToEventListenerSetMap;

class OmEventManager : boost::noncopyable, public QObject {
public:
	static OmEventManager* Instance();
	static void Delete();

	//static accessors
	static void AddListener(OmEventClass, OmEventListener*);
	static void RemoveListener(OmEventClass, OmEventListener*);

	//event accessors
	static bool SendEvent(OmEvent&);
	static void PostEvent(OmEvent*);
	static void FlushPostEvents();

private:
	OmEventManager(){}
	~OmEventManager(){}

	//singleton
	static OmEventManager* mspInstance;

	//map events
	EventClassToEventListenerSetMap mEventClassToListernsMap;

	//qtevent handler
	bool event(QEvent *event);	//customEvent

	//EventListener accessors
	void AddEventListener(OmEventClass, OmEventListener*);
	void RemoveEventListener(OmEventClass, OmEventListener*);
};

#endif
