#include "events/details/executeOnMainEvent.h"

namespace om {
namespace event {

const QEvent::Type ExecuteOnMainEvent::EXECUTE_ON_MAIN =
    (QEvent::Type)QEvent::registerEventType();
}
}
