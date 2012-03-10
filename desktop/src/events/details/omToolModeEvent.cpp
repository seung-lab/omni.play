#include <assert.h>
#include "omToolModeEvent.h"

/*
 * Dispatch event based on event type.
 */
void OmToolModeEvent::Dispatch(OmEventListener * pListener)
{
    //cast to proper listener
    OmToolModeEventListener *p_cast_listener =
        dynamic_cast<OmToolModeEventListener*>(pListener);
    assert(p_cast_listener);

    switch(type()) {
    case OmToolModeEvent::TOOL_MODE_CHANGE:
        p_cast_listener->ToolModeChangeEvent();
        return;

    default:
        throw OmArgException("unknown event type");
    }
}
