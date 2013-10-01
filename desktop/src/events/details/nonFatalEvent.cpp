#include "events/details/nonFatalEvent.h"

namespace om {
namespace event {

const QEvent::Type NonFatalEvent::NON_FATAL_ERROR_OCCURED =
    (QEvent::Type)QEvent::registerEventType();
}
}
