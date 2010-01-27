
#include "omView3dEvent.h"
#include "system/omEventManager.h"

#define DEBUG 0

OmView3dEvent::OmView3dEvent(QEvent::Type type)
 : OmEvent(type, CLASS)
{
}

/*
 *	Dispatch event based on event type.
 */
void
 OmView3dEvent::Dispatch(OmEventListener * pListener)
{

	//cast to proper listener
	OmView3dEventListener *p_cast_listener = dynamic_cast < OmView3dEventListener * >(pListener);
	assert(p_cast_listener);

	switch (type()) {

	case OmView3dEvent::REDRAW:
		p_cast_listener->View3dRedrawEvent(this);
		return;

	case OmView3dEvent::UPDATE_PREFERENCES:
		//p_cast_listener->View3dUpdatePreferencesEvent( this );
		return;

	default:
		assert(false);
	}
}
