#include "events/details/omRefreshMSTthreshold.h"

OmRefreshMSTthresholdEvent::OmRefreshMSTthresholdEvent()
    : OmEvent(REFRESH_MST_THRESHOLD, CLASS)
{}

/*
 * Dispatch event based on event type.
 */
void OmRefreshMSTthresholdEvent::Dispatch(OmEventListener* listenerBase)
{
    //cast to proper listener
    OmRefreshMSTthresholdEventListener* listener =
        dynamic_cast<OmRefreshMSTthresholdEventListener*>(listenerBase);

    assert(listener);

    switch (type()) {
    case OmRefreshMSTthresholdEvent::REFRESH_MST_THRESHOLD:
        listener->RefreshMSTEvent(this);
        return;

    default:
        throw om::ArgException("unknown event type");
    }
}
