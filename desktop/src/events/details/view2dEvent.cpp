#include "events/details/view2dEvent.h"

namespace om {
namespace event {

const QEvent::Type View2dEvent::VIEW_BOX_CHANGE =
    (QEvent::Type)QEvent::registerEventType();
const QEvent::Type View2dEvent::VIEW_CENTER_CHANGE =
    (QEvent::Type)QEvent::registerEventType();
const QEvent::Type View2dEvent::VIEW_POS_CHANGE =
    (QEvent::Type)QEvent::registerEventType();
const QEvent::Type View2dEvent::REDRAW =
    (QEvent::Type)QEvent::registerEventType();
const QEvent::Type View2dEvent::REDRAW_BLOCKING =
    (QEvent::Type)QEvent::registerEventType();
const QEvent::Type View2dEvent::COORD_SYSTEM_CHANGE =
    (QEvent::Type)QEvent::registerEventType();
}
}
