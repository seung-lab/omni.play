#pragma once

/*
 *
 *  Brett Warne - bwarne@mit.edu - 3/14/09
 */

#include "events/details/omEvent.h"

class OmToolModeEvent : public OmEvent {

 public:
  OmToolModeEvent(QEvent::Type type) : OmEvent(type, CLASS) {}

  void Dispatch(OmEventListener *);

  //class
  static const OmEventClass CLASS = OM_TOOL_MODE_EVENT_CLASS;

  //events
  static const QEvent::Type TOOL_MODE_CHANGE = (QEvent::Type)(CLASS);
};

class OmToolModeEventListener : public OmEventListener {
 public:
  OmToolModeEventListener() : OmEventListener(OmToolModeEvent::CLASS) {}

  virtual void ToolModeChangeEvent() = 0;
};
