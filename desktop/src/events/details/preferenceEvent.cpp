#include "events/details/preferenceEvent.h"

namespace om {
namespace event {

const QEvent::Type PreferenceEvent::PREFERENCE_CHANGE =
    (QEvent::Type) QEvent::registerEventType();
}
}
