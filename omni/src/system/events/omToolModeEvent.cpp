
#include "omToolModeEvent.h"

#define DEBUG 0




OmToolModeEvent::OmToolModeEvent(QEvent::Type type) 
: OmEvent(type, CLASS)  { 

}




/*
 *	Dispatch event based on event type.
 */

void 
OmToolModeEvent::Dispatch(OmEventListener *pListener) {
	
	//cast to proper listener
	OmToolModeEventListener *p_cast_listener = dynamic_cast<OmToolModeEventListener *>(pListener);
	assert(p_cast_listener);
	
	switch(type()) {
			
		case OmToolModeEvent::TOOL_MODE_CHANGE:
			p_cast_listener->ToolModeChangeEvent( this );
			return;
			
		default:
			assert(false);
	}
	
}

