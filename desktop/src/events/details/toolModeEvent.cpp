#include "events/details/toolModeEvent.h"

namespace om {
namespace event {

const QEvent::Type ToolModeEvent::TOOL_MODE_CHANGE =
    (QEvent::Type)QEvent::registerEventType();
}
}
