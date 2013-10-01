#include "view3dEvent.h"

namespace om {
namespace event {

const QEvent::Type View3dEvent::REDRAW =
    (QEvent::Type)QEvent::registerEventType();
const QEvent::Type View3dEvent::RECENTER =
    (QEvent::Type)QEvent::registerEventType();
}
}
