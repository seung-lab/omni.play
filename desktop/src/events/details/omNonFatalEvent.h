#pragma once

#include "events/details/omEvent.h"

class OmNonFatalEvent : public OmEvent {
 private:
  const QString err_;

 public:
  OmNonFatalEvent(const QString& err);

  void Dispatch(OmEventListener*);

  //class
  static const OmEventClass CLASS = OM_NON_FATAL_EVENT_CLASS;

  //events
  static const QEvent::Type NON_FATAL_ERROR_OCCURED = (QEvent::Type)(CLASS);

  QString Error() const { return err_; }
};

class OmNonFatalEventListener : public OmEventListener {

 public:
  OmNonFatalEventListener() : OmEventListener(OmNonFatalEvent::CLASS) {}
  ;

  virtual void NonFatalEvent(OmNonFatalEvent*) = 0;
};
