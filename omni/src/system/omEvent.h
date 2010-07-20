#ifndef OM_EVENT_H
#define OM_EVENT_H

/*
 *
 *	Brett Warne - bwarne@mit.edu - 4/6/09
 */

#include <QObject>
#include <QEvent>
#include "common/omStd.h"

enum OmEventClass { OM_VIEW_EVENT_CLASS = 2000,
		    OM_SEGMENT_EVENT_CLASS = 3000,
		    OM_VIEW_3D_EVENT_CLASS = 4000,
		    OM_SYSTEM_MODE_EVENT_CLASS = 5000,
		    OM_TOOL_MODE_EVENT_CLASS = 6000,
		    OM_PROGRESS_EVENT_CLASS = 7000,
		    OM_PREFERENCE_EVENT_CLASS = 8000,
		    OM_VOXEL_EVENT_CLASS = 9000,
		    OM_ALERT_EVENT_CLASS = 10000};

class OmEventListener;

class OmEvent : public QEvent {
public:	
	virtual void Dispatch(OmEventListener *) = 0;
	
	const OmEventClass ChildClass;
	
protected:
	OmEvent(QEvent::Type type, OmEventClass eventClass) 
		: QEvent(type), ChildClass(eventClass) { };
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
	OmEventListener() { }
	OmEventClass mEventClass;
};
	


#endif
