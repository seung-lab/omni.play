#include "omUserInterfaceEvent.h"

#include <assert.h>

OmUserInterfaceEvent::OmUserInterfaceEvent(QEvent::Type type)
    : OmEvent(type, CLASS)
{}

OmUserInterfaceEvent::OmUserInterfaceEvent(QWidget* widget, const QString& title)
    : OmEvent(UPDATE_SEGMENT_PROP_WIDGET, CLASS)
    , widget_(widget)
    , title_(title)
{}

/*
 * Dispatch event based on event type.
 */
void OmUserInterfaceEvent::Dispatch(OmEventListener* pListener)
{

    //cast to proper listener
    OmUserInterfaceEventListener *p_cast_listener =
        dynamic_cast<OmUserInterfaceEventListener*>(pListener);

    assert(p_cast_listener);

    switch (type()) {

    case OmUserInterfaceEvent::UPDATE_SEGMENT_PROP_WIDGET:
        p_cast_listener->UpdateSegmentPropWidgetEvent(this);
        return;

    default:
        throw OmArgException("unknown event type");
    }
}
