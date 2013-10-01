#include "events/details/uiEvent.h"

namespace om {
namespace event {

const QEvent::Type UIEvent::UPDATE_SEGMENT_PROP_WIDGET =
    (QEvent::Type)QEvent::registerEventType();
}
}
