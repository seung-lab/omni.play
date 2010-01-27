#ifndef OM_VIEW_3D_EVENT_CLASS_H
#define OM_VIEW_3D_EVENT_CLASS_H

/*
 *
 *	Brett Warne - bwarne@mit.edu - 3/14/09
 */

#include <common/omStd.h>
#include "system/omEvent.h"

/*
 *	View Event
 */
class OmView3dEvent:public OmEvent {

 public:
	OmView3dEvent(QEvent::Type type);
	void Dispatch(OmEventListener *);

	//class
	static const OmEventClass CLASS = OM_VIEW_3D_EVENT_CLASS;
	//events
	static const QEvent::Type REDRAW = (QEvent::Type) (CLASS);
	static const QEvent::Type UPDATE_PREFERENCES = (QEvent::Type) (CLASS + 1);
};

/*
 *	View Event Listener
 */
class OmView3dEventListener:public OmEventListener {

 public:
	OmView3dEventListener():OmEventListener(OmView3dEvent::CLASS) {
		//cout << "OmView3dEventListener()" << this << endl;
	};

	virtual void View3dRedrawEvent(OmView3dEvent * event) {
	};
	virtual void View3dUpdatePreferencesEvent(OmView3dEvent * event) {
	};
};

#endif
