#include "events/details/mstEvent.h"

namespace om {
namespace event {

const QEvent::Type MSTEvent::REFRESH_MST_THRESHOLD =
    (QEvent::Type) QEvent::registerEventType();
}
}
