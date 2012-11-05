#include "events/details/omNonFatalEvent.h"

OmNonFatalEvent::OmNonFatalEvent(const QString& err)
    : OmEvent(NON_FATAL_ERROR_OCCURED, CLASS)
    , err_(err)
{}

/*
 * Dispatch event based on event type.
 */
void OmNonFatalEvent::Dispatch(OmEventListener* listenerBase)
{
    //cast to proper listener
    OmNonFatalEventListener* listener =
        dynamic_cast<OmNonFatalEventListener*>(listenerBase);

    assert(listener);

    switch (type()) {
    case OmNonFatalEvent::NON_FATAL_ERROR_OCCURED:
        listener->NonFatalEvent(this);
        return;

    default:
        throw ArgException("unknown event type");
    }
}
