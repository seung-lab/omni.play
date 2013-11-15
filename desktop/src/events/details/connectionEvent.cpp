#include "events/details/connectionEvent.h"

namespace om {
namespace event {

const QEvent::Type ConnectionEvent::CONNECTION_CHANGE =
    (QEvent::Type)QEvent::registerEventType();
}
}
