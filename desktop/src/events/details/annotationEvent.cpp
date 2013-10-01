#include "events/details/annotationEvent.h"

namespace om {
namespace event {

const QEvent::Type AnnotationEvent::OBJECT_MODIFIED =
    (QEvent::Type)QEvent::registerEventType();
}
}
