#pragma once
#include "precomp.h"

#include "events/details/listener.h"
#include "actions/omSelectSegmentParams.hpp"

namespace om {
namespace event {

class AnnotationEvent;
class NonFatalEvent;
class PreferenceEvent;
class MSTEvent;
class SegmentEvent;
class UIEvent;
class ExecuteOnMainEvent;

struct AnnotationEventListener : public Listener {
  AnnotationEventListener() : Listener(Klass::annotation) {}
  virtual void AnnotationModificationEvent(AnnotationEvent*) = 0;
};

struct NonFatalEventListener : public Listener {
  NonFatalEventListener() : Listener(Klass::nonFatalEvent) {}
  virtual void NonFatalEvent(NonFatalEvent*) = 0;
};

struct PreferenceEventListener : public Listener {
  PreferenceEventListener() : Listener(Klass::preference) {}
  virtual void PreferenceChangeEvent(PreferenceEvent*) = 0;
};

struct MSTEventListener : public Listener {
  MSTEventListener() : Listener(Klass::mst) {}
  virtual void RefreshMSTEvent(MSTEvent*) = 0;
};

struct SegmentEventListener : public Listener {
  SegmentEventListener() : Listener(Klass::segment) {}
  // add/remove segment, change state, change selection
  virtual void SegmentModificationEvent(SegmentEvent*) = 0;
  virtual void SegmentGUIlistEvent(SegmentEvent*) = 0;
  virtual void SegmentSelectedEvent(SegmentEvent*, std::shared_ptr<OmSelectSegmentsParams> params) = 0;
  virtual void SegmentBrushEvent(SegmentEvent*) = 0;
};

struct ToolModeEventListener : public Listener {
  ToolModeEventListener() : Listener(Klass::tool) {}
  virtual void ToolModeChangeEvent() = 0;
};

struct UIEventListener : public Listener {
  UIEventListener() : Listener(Klass::ui) {}
  virtual void UpdateSegmentPropWidgetEvent(UIEvent*) = 0;
};

struct View2dEventListener : public Listener {
  View2dEventListener() : Listener(Klass::view2d) {}
  virtual void ViewBoxChangeEvent() = 0;
  virtual void ViewCenterChangeEvent() = 0;
  virtual void ViewPosChangeEvent() = 0;
  virtual void ViewRedrawEvent() = 0;
  virtual void ViewBlockingRedrawEvent() = 0;
  virtual void CoordSystemChangeEvent() = 0;
};

struct View3dEventListener : public Listener {
  View3dEventListener() : Listener(Klass::view3d) {}
  virtual void View3dRedrawEvent() = 0;
  virtual void View3dRecenter() = 0;
};

struct TaskEventListener : public Listener {
  TaskEventListener() : Listener(Klass::task) {}
  virtual void TaskChangeEvent() {}
  virtual void TaskStartedEvent() {}
};

struct ConnectionEventListener : public Listener {
  ConnectionEventListener() : Listener(Klass::connection) {}
  virtual void ConnectionChangeEvent() = 0;
};

struct ExecuteOnMainEventListener : public Listener {
  ExecuteOnMainEventListener() : Listener(Klass::execOnMain) {}
  virtual void ExecuteOnMainEvent(ExecuteOnMainEvent*) = 0;
};
}
}  // om::event::
