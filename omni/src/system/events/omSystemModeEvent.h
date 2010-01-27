#ifndef OM_SYSTEM_MODE_EVENT_H
#define OM_SYSTEM_MODE_EVENT_H

/*
 *
 *	Brett Warne - bwarne@mit.edu - 3/14/09
 */

#include <common/omStd.h>
#include <vmmlib/vmmlib.h>
using namespace vmml;

#include "system/omEvent.h"

class OmSystemModeEvent:public OmEvent {

 public:
	OmSystemModeEvent(QEvent::Type type);
	void Dispatch(OmEventListener *);

	//class
	static const OmEventClass CLASS = OM_SYSTEM_MODE_EVENT_CLASS;
	//events
	static const QEvent::Type SYSTEM_MODE_CHANGE = (QEvent::Type) (CLASS);
};

class OmSystemModeEventListener:public OmEventListener {

 public:
	OmSystemModeEventListener():OmEventListener(OmSystemModeEvent::CLASS) {
	};

	virtual void SystemModeChangeEvent(OmSystemModeEvent * event) {
	};

};

#endif
