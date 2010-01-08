#ifndef OM_VIEW_EVENT_CLASS_H
#define OM_VIEW_EVENT_CLASS_H

/*
 *
 *	Brett Warne - bwarne@mit.edu - 3/14/09
 */



#include "system/omEvent.h"

#include "common/omStd.h"
#include "volume/omVolumeTypes.h"

#include <vmmlib/vmmlib.h>
using namespace vmml;



/*
 *	View Event
 */
class OmViewEvent : public OmEvent {

public:
	OmViewEvent(QEvent::Type type);
	void Dispatch(OmEventListener *);
	
	
	//class
	static const OmEventClass CLASS = OM_VIEW_EVENT_CLASS;
	//events
	static const QEvent::Type VIEW_BOX_CHANGE = (QEvent::Type) (CLASS);
	static const QEvent::Type VIEW_CENTER_CHANGE = (QEvent::Type) (CLASS + 1);
	static const QEvent::Type VIEW_POS_CHANGE = (QEvent::Type) (CLASS + 2);
	
	static const QEvent::Type REDRAW = (QEvent::Type) (CLASS + 3);

	
};






/*
 *	View Event Listener
 */
class OmViewEventListener : public OmEventListener {
	
public:	
	OmViewEventListener() : OmEventListener(OmViewEvent::CLASS) { };
	
	virtual void ViewBoxChangeEvent(OmViewEvent *event) { };
	virtual void ViewCenterChangeEvent(OmViewEvent *event) { };
	virtual void ViewPosChangeEvent(OmViewEvent *event) { };
	
	virtual void ViewRedrawEvent(OmViewEvent *event) { };
};



#endif
