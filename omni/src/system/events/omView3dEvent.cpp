#include "omView3dEvent.h"
#include "system/events/omEventManager.h"

OmView3dEvent::OmView3dEvent(QEvent::Type type)
 : OmEvent(type, CLASS)
{}

/*
 *	Dispatch event based on event type.
 */
void OmView3dEvent::Dispatch(OmEventListener * pListener)
{
	//cast to proper listener
	OmView3dEventListener *p_cast_listener =
		dynamic_cast < OmView3dEventListener * >(pListener);
	assert(p_cast_listener);

	switch (type()) {
	case OmView3dEvent::REDRAW:
		p_cast_listener->View3dRedrawEvent();
		return;
	case OmView3dEvent::REDRAW_CACHE:
		p_cast_listener->View3dRedrawEventFromCache();
		return;
	case OmView3dEvent::UPDATE_PREFERENCES:
		//p_cast_listener->View3dUpdatePreferencesEvent( this );
		return;
	case OmView3dEvent::RECENTER:
		p_cast_listener->View3dRecenter();
		return;
	default:
		assert(false);
	}
}
