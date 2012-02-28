#include "omView3dEvent.h"
#include "events/details/omEventManager.h"

OmView3dEvent::OmView3dEvent(QEvent::Type type)
    : OmEvent(type, CLASS)
{}

/*
 * Dispatch event based on event type.
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

    case OmView3dEvent::RECENTER:
        p_cast_listener->View3dRecenter();
        return;

    default:
        throw OmArgException("unknown event type");
    }
}
