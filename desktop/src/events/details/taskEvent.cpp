#include "events/details/taskEvent.h"

namespace om {
namespace event {

const QEvent::Type TaskEvent::TASK_CHANGE =
    (QEvent::Type)QEvent::registerEventType();
}
}
