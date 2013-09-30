#pragma once

/*
 *
 * Brett Warne - bwarne@mit.edu - 3/14/09
 */

#include "events/details/omEvent.h"
#include "common/common.h"

namespace om {
namespace events {

class annotationEvent : public OmEvent {

 public:
  annotationEvent(QEvent::Type type);

  void Dispatch(OmEventListener*);

  static const OmEventClass CLASS = OM_ANNOTATION_EVENT_CLASS;

  static const QEvent::Type ANNOTATION_OBJECT_MODIFICATION =
      (QEvent::Type)(CLASS);

 private:

};

class annotationEventListener : public OmEventListener {

 public:
  annotationEventListener() : OmEventListener(annotationEvent::CLASS) {}

  //add/remove Annotation, change state, change selection
  virtual void AnnotationModificationEvent(annotationEvent* event) = 0;
};

}  // namespace events
}  // namespace om