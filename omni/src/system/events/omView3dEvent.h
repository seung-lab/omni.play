#ifndef OM_VIEW_3D_EVENT_CLASS_H
#define OM_VIEW_3D_EVENT_CLASS_H

/*
 *
 *	Brett Warne - bwarne@mit.edu - 3/14/09
 */



#include <common/omStd.h>
#include "system/events/omEvent.h"

class OmView3dEvent : public OmEvent {
public:
	OmView3dEvent(QEvent::Type type);
	void Dispatch(OmEventListener *);

	//class
	static const OmEventClass CLASS = OM_VIEW_3D_EVENT_CLASS;

	//events
	static const QEvent::Type REDRAW =
		static_cast<QEvent::Type>(CLASS);

	static const QEvent::Type UPDATE_PREFERENCES =
		static_cast<QEvent::Type>(CLASS + 1);

	static const QEvent::Type REDRAW_CACHE =
		static_cast<QEvent::Type>(CLASS + 2);
};

class OmView3dEventListener : public OmEventListener {
public:
	OmView3dEventListener()
		: OmEventListener(OmView3dEvent::CLASS)
	{};

	virtual void View3dRedrawEvent() = 0;
	virtual void View3dRedrawEventFromCache() = 0;
	virtual void View3dUpdatePreferencesEvent() = 0;
};



#endif
