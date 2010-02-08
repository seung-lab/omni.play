
#include "omAlertEvent.h"

#define DEBUG 1

OmAlertEvent::OmAlertEvent(QEvent::Type type, string text, string secondText)
 : OmEvent(type, CLASS)
{
	DOUT("OmAlertEvent created");
	mText = text;
	mMoreText = secondText;
}

/*
 *	Dispatch event based on event type.
 */

void
 OmAlertEvent::Dispatch(OmEventListener * pListener)
{

	//cast to proper listener
	OmAlertEventListener *p_cast_listener = dynamic_cast < OmAlertEventListener * >(pListener);
	assert(p_cast_listener);

	switch (type()) {

	case OmAlertEvent::ALERT_NOTIFY:
		p_cast_listener->AlertNotifyEvent(this);
		return;

	default:
		assert(false);
	}

}
