#pragma once
#include "precomp.h"

#include "common/common.h"

class OmSelectSegmentsParams;
class SegmentDataWrapper;
class SegmentationDataWrapper;

namespace om {
namespace event {

void ToolChange();
void Redraw2d();
void Redraw2dBlocking();
void Redraw3d();
void SegmentGUIlist(const SegmentationDataWrapper& sdw, const bool stayOnPage);
void SegmentModified();
void SegmentModified(std::shared_ptr<OmSelectSegmentsParams> params);
void SegmentSelected();
void ViewCenterChanged();
void View3dRecenter();
void ViewBoxChanged();
void ViewPosChanged();
void CoordSystemChanged();
void PreferenceChange(const int key);

void TaskChange();
void TaskStarted();

void BeforeConnectionChange();
void ConnectionChanged();

void UpdateSegmentPropBox(QWidget* widget, const QString& title);

// used to update GUI spin box value from do/undo action
void RefreshMSTthreshold();

void NonFatalEventOccured(const QString err);

void AnnotationObjectModified();

void ExecuteOnMain(std::function<void()>);
}
}  // om::event::
