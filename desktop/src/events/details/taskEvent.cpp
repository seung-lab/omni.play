#include "events/details/taskEvent.h"

namespace om {
namespace event {

const QEvent::Type TaskEvent::TASK_CHANGE =
    (QEvent::Type)QEvent::registerEventType();
const QEvent::Type TaskEvent::TASK_STARTED =
    (QEvent::Type)QEvent::registerEventType();
}
}
