#ifndef OM_VIEW_EVENT_CLASS_H
#define OM_VIEW_EVENT_CLASS_H

/*
 *
 *	Brett Warne - bwarne@mit.edu - 3/14/09
 */

#include "system/events/omEvent.h"
#include "common/omStd.h"

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

	virtual void ViewBoxChangeEvent() = 0;
	virtual void ViewCenterChangeEvent() = 0;
	virtual void ViewPosChangeEvent() = 0;
	virtual void ViewRedrawEvent() = 0;
};

#endif
