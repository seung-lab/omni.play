#ifndef OM_TOOL_MODE_EVENT_H
#define OM_TOOL_MODE_EVENT_H

/*
 *
 *	Brett Warne - bwarne@mit.edu - 3/14/09
 */


#include <common/omStd.h>
#include <vmmlib/vmmlib.h>
using namespace vmml;

#include "system/omEvent.h"
	

class OmToolModeEvent : public OmEvent {

public:
	OmToolModeEvent(QEvent::Type type);	
	void Dispatch(OmEventListener *);
	
	
	//class
	static const OmEventClass CLASS  = OM_TOOL_MODE_EVENT_CLASS;
	//events
	static const QEvent::Type TOOL_MODE_CHANGE = (QEvent::Type) (CLASS);
};



class OmToolModeEventListener : public OmEventListener {
	
public:	
	OmToolModeEventListener() : OmEventListener(OmToolModeEvent::CLASS) { };
	
	virtual void ToolModeChangeEvent(OmToolModeEvent *event) { };

};



#endif
