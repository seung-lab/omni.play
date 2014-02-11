#include "events/details/annotationEvent.h"
#include "events/details/event.h"
#include "events/details/manager.h"
#include "events/details/mstEvent.h"
#include "events/details/nonFatalEvent.h"
#include "events/details/preferenceEvent.h"
#include "events/details/segmentEvent.h"
#include "events/details/toolModeEvent.h"
#include "events/details/uiEvent.h"
#include "events/details/view2dEvent.h"
#include "events/details/view3dEvent.h"
#include "events/details/taskEvent.h"
#include "events/details/connectionEvent.h"
#include "events/events.h"

namespace om {
namespace event {

void ToolChange() {
  Manager::Post(new ToolModeEvent(ToolModeEvent::TOOL_MODE_CHANGE));
}

void Redraw2d() { Manager::Post(new View2dEvent(View2dEvent::REDRAW)); }

void Redraw2dBlocking() {
  Manager::Post(new View2dEvent(View2dEvent::REDRAW_BLOCKING));
}

void Redraw3d() { Manager::Post(new View3dEvent(View3dEvent::REDRAW)); }

void SegmentSelected() {
  Manager::Post(new SegmentEvent(SegmentEvent::SEGMENT_SELECTED));
}

void SegmentGUIlist(const SegmentationDataWrapper& sdw, const bool stayOnPage) {
  Manager::Post(
      new SegmentEvent(SegmentEvent::SEGMENT_GUI_LIST, sdw, stayOnPage));
}

void SegmentModified() {
  Manager::Post(new SegmentEvent(SegmentEvent::MODIFIED));
}

void SegmentModified(std::shared_ptr<OmSelectSegmentsParams> params) {
  Manager::Post(new SegmentEvent(SegmentEvent::MODIFIED, params));
}

void ViewCenterChanged() {
  Manager::Post(new View2dEvent(View2dEvent::VIEW_CENTER_CHANGE));
}

void View3dRecenter() { Manager::Post(new View3dEvent(View3dEvent::RECENTER)); }

void ViewBoxChanged() {
  Manager::Post(new View2dEvent(View2dEvent::VIEW_BOX_CHANGE));
}

void ViewPosChanged() {
  Manager::Post(new View2dEvent(View2dEvent::VIEW_POS_CHANGE));
}

void CoordSystemChanged() {
  Manager::Post(new View2dEvent(View2dEvent::COORD_SYSTEM_CHANGE));
}

void PreferenceChange(const int key) {
  Manager::Post(new PreferenceEvent(PreferenceEvent::PREFERENCE_CHANGE, key));
}

void TaskChange() { Manager::Post(new TaskEvent(TaskEvent::TASK_CHANGE)); }
void TaskStarted() { Manager::Post(new TaskEvent(TaskEvent::TASK_STARTED)); }

void ConnectionChanged() {
  Manager::Post(new ConnectionEvent(ConnectionEvent::CONNECTION_CHANGE));
}

void UpdateSegmentPropBox(QWidget* widget, const QString& title) {
  Manager::Post(new UIEvent(widget, title));
}

void RefreshMSTthreshold() { Manager::Post(new MSTEvent()); }

void NonFatalEventOccured(const QString err) {
  Manager::Post(new NonFatalEvent(err));
}

void AnnotationObjectModified() {
  Manager::Post(new AnnotationEvent(AnnotationEvent::OBJECT_MODIFIED));
}
}
}  //
