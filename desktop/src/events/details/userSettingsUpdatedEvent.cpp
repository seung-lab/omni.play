#include "events/details/userSettingsUpdatedEvent.h"

namespace om {
namespace event {
const QEvent::Type UserSettingsUpdatedEvent::USER_SETTINGS_UPDATED =
    (QEvent::Type) QEvent::registerEventType();
}
}
