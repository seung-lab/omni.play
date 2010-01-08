
#include "omViewEvent.h"


#define DEBUG 0



/*
 *	Construct and initialize references.
 */
OmViewEvent::OmViewEvent(QEvent::Type type)
: OmEvent(type, CLASS) {

}



/*
 *	Dispatch event based on event type.
 */
void 
OmViewEvent::Dispatch(OmEventListener *pListener) {
	
	//cast to proper listener
	OmViewEventListener *p_cast_listener = dynamic_cast<OmViewEventListener *>(pListener);
	assert(p_cast_listener);
	
	switch(type()) {
			
		case OmViewEvent::VIEW_BOX_CHANGE:
			p_cast_listener->ViewBoxChangeEvent( this );
			return;
			
		case OmViewEvent::VIEW_CENTER_CHANGE:
			p_cast_listener->ViewCenterChangeEvent( this );
			return;
		
		case OmViewEvent::VIEW_POS_CHANGE:
			p_cast_listener->ViewPosChangeEvent( this );
			return;
		
		case OmViewEvent::REDRAW:
			p_cast_listener->ViewRedrawEvent( this );
			return;
		default:
			assert(false);
	}
	
}


