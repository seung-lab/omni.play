#ifndef OM_ALERT_EVENT_CLASS_H
#define OM_ALERT_EVENT_CLASS_H

/*
 *
 *	Rachel Shearer - rshearer@mit.edu
 */



#include "system/omEvent.h"

#include "common/omStd.h"
#include "volume/omVolumeTypes.h"

#include <vmmlib/vmmlib.h>
using namespace vmml;



/*
 *	View Event
 */
class OmAlertEvent : public OmEvent {

public:
	OmAlertEvent(QEvent::Type type, string text, string secondText);
	void Dispatch(OmEventListener *);
	
	
	//class
	static const OmEventClass CLASS = OM_ALERT_EVENT_CLASS;
	//events
	static const QEvent::Type ALERT_NOTIFY = (QEvent::Type) (CLASS);
	
	
	string GetText() { return mText; }
	string GetMoreText() { return mMoreText; }
	
private:
	string mText;
	string mMoreText;
	
};






/*
 *	Alert Event Listener
 */
class OmAlertEventListener : public OmEventListener {
	
public:	
	OmAlertEventListener() : OmEventListener(OmAlertEvent::CLASS) { };
	
	virtual void AlertNotifyEvent(OmAlertEvent *event) { };
};



#endif
