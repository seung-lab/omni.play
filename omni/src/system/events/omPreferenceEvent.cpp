
#include "omPreferenceEvent.h"

#define DEBUG 0




OmPreferenceEvent::OmPreferenceEvent(QEvent::Type type, int pref)
: OmEvent(type, CLASS)  { 
	mPreference = pref;
}




/*
 *	Dispatch event based on event type.
 */

void 
OmPreferenceEvent::Dispatch(OmEventListener *pListener) {
	
	//cast to proper listener
	OmPreferenceEventListener *p_cast_listener = dynamic_cast<OmPreferenceEventListener *>(pListener);
	assert(p_cast_listener);
	
	switch(type()) {
			
		case OmPreferenceEvent::PREFERENCE_CHANGE:
			p_cast_listener->PreferenceChangeEvent( this );
			return;
			
		default:
			assert(false);
	}
	
}

