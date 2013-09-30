#include "actions/omSelectSegmentParams.hpp"
#include "events/details/annotationEvent.h"

namespace om {
namespace events {

annotationEvent::annotationEvent(QEvent::Type type) : OmEvent(type, CLASS) {}

/*
 *  Dispatch event based on event type.
 */

void annotationEvent::Dispatch(OmEventListener* pListener) {
  //cast to proper listener
  annotationEventListener* p_cast_listener =
      dynamic_cast<annotationEventListener*>(pListener);

  assert(p_cast_listener);

  switch (type()) {
    case annotationEvent::ANNOTATION_OBJECT_MODIFICATION:
      p_cast_listener->AnnotationModificationEvent(this);
      return;

    default:
      throw om::ArgException("unknown event type");
  }
}

}  // namespace events
}  // namespace om