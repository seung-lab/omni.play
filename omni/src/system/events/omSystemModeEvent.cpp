
#include "omSystemModeEvent.h"

#define DEBUG 0

OmSystemModeEvent::OmSystemModeEvent(QEvent::Type type)
 : OmEvent(type, CLASS)
{

}

/*
 *	Dispatch event based on event type.
 */

void
 OmSystemModeEvent::Dispatch(OmEventListener * pListener)
{

	//cast to proper listener
	OmSystemModeEventListener *p_cast_listener = dynamic_cast < OmSystemModeEventListener * >(pListener);
	assert(p_cast_listener);

	switch (type()) {

	case OmSystemModeEvent::SYSTEM_MODE_CHANGE:
		p_cast_listener->SystemModeChangeEvent(this);
		return;

	default:
		assert(false);
	}

}
