#pragma once

#include "actions/omActions.h"
#include "gui/widgets/omAskYesNoQuestion.hpp"
#include "gui/widgets/omSegmentContextMenu.h"
#include "gui/widgets/omTellInfo.hpp"
#include "landmarks/omLandmarks.hpp"
#include "segment/actions/omSplitSegmentRunner.hpp"
#include "view2d/brush/omBrushSelect.hpp"
#include "view2d/omFillTool.hpp"
#include "view2d/omMouseEventUtils.hpp"
#include "view2d/omView2d.h"
#include "view2d/omView2dState.hpp"
#include <annotation/annotation.h>

class OmMouseEventPress {
 private:
  OmView2d* const v2d_;
  OmView2dState* const state_;

  OmSegmentContextMenu segmentContextMenu_;

  bool controlKey_;
  bool altKey_;
  bool shiftKey_;
  bool leftMouseButton_;
  bool rightMouseButton_;
  bool middleMouseButton_;
  om::tool::mode tool_;
  QMouseEvent* event_;
  om::globalCoord dataClickPoint_;

  friend class OmMouseEventState;

 public:
  OmMouseEventPress(OmView2d* v2d, OmView2dState* state)
      : v2d_(v2d), state_(state) {}

  void Press(QMouseEvent* event) {
    setState(event);

    state_->SetMousePanStartingPt(
        om::screenCoord(event->x(), event->y(), state_));

    if (leftMouseButton_) {
      if (om::tool::SPLIT == tool_) {
        doFindAndSplitSegment();
        v2d_->Redraw();
        return;
      }

      if (om::tool::SHATTER == tool_) {
        doFindAndShatterSegment();
        v2d_->Redraw();
        return;
      }

      const bool doCrosshair = controlKey_ && om::tool::PAN == tool_;

      if (doCrosshair) {
        mouseSetCrosshair();

      } else {
        mouseLeftButton();
      }

    } else if (rightMouseButton_) {
      if (controlKey_) {
        mouseSelectSegment();

      } else {
        mouseShowSegmentContextMenu();
      }
    } else if (middleMouseButton_) {
      state_->OverrideToolModeForPan(true);
    }
  }

 private:
  void setState(QMouseEvent* event) {
    event_ = event;

    controlKey_ = event->modifiers() & Qt::ControlModifier;
    shiftKey_ = event->modifiers() & Qt::ShiftModifier;
    altKey_ = event->modifiers() & Qt::AltModifier;

    leftMouseButton_ = event->buttons() & Qt::LeftButton;
    rightMouseButton_ = event->buttons() & Qt::RightButton;
    middleMouseButton_ = event->buttons() & Qt::MiddleButton;

    tool_ = OmStateManager::GetToolMode();
    om::screenCoord clicked(event->x(), event->y(), state_);
    dataClickPoint_ = clicked.toGlobalCoord();
  }

  void doFindAndSplitSegment() {
    boost::optional<SegmentDataWrapper> sdw = getSelectedSegment();

    if (!sdw) {
      return;
    }

    OmSplitSegmentRunner::FindAndSplitSegments(
        *sdw, state_->getViewGroupState(), dataClickPoint_);
  }

  void doFindAndShatterSegment() {
    boost::optional<SegmentDataWrapper> sdw = getSelectedSegment();

    if (!sdw) {
      return;
    }

    OmActions::ShatterSegment(sdw->GetSegment());
    state_->getViewGroupState()->GetToolBarManager()->SetShatteringOff();
    OmStateManager::SetOldToolModeAndSendEvent();
  }

  void doFindAndCutSegment() {
    boost::optional<SegmentDataWrapper> sdw = getSelectedSegment();

    if (!sdw) {
      return;
    }

    OmActions::CutSegment(*sdw);
  }

  void mouseSetCrosshair() {
    v2d_->Redraw();
    setDepth();
    state_->getViewGroupState()->setTool(om::tool::PAN);
  }

  void setDepth() {
    const om::screenCoord screenc =
        om::screenCoord(event_->x(), event_->y(), state_);
    const om::globalCoord newloc = screenc.toGlobalCoord();
    state_->setLocation(newloc);

    OmEvents::ViewCenterChanged();
  }

  void mouseLeftButton() {
    if (controlKey_) {
      state_->OverrideToolModeForPan(true);
      return;
    }

    switch (tool_) {
      case om::tool::SELECT:
        state_->setScribbling(true);
        selectSegments();
        break;
      case om::tool::PAN:
        return;
      case om::tool::PAINT:
        state_->setScribbling(true);
        paint();
        break;
      case om::tool::ERASE:
        state_->setScribbling(true);
        erase();
        break;
      case om::tool::FILL:
        fill();
        break;
      case om::tool::LANDMARK:
        state_->getViewGroupState()->Landmarks()
            .Add(getSelectedSegment(), dataClickPoint_);
        break;
      case om::tool::CUT:
        doFindAndCutSegment();
        break;
      case om::tool::ANNOTATE:
        addAnnotation();
        break;
      case om::tool::KALINA:
        kalina();
        break;
      default:
        return;
    }

    state_->SetLastDataPoint(dataClickPoint_);

    v2d_->Redraw();
  }

  void selectSegments() {
    om::AddOrSubtract addOrSubtractSegments = altKey_ ? om::SUBTRACT : om::ADD;

    OmBrushSelect::SelectByClick(state_, dataClickPoint_,
                                 addOrSubtractSegments);
  }

  void erase() { OmBrushPaint::PaintByClick(state_, dataClickPoint_, 0); }

  void paint() {
    SegmentDataWrapper sdw = OmSegmentSelected::GetSegmentForPainting();
    if (!sdw.IsSegmentValid()) {
      tellUserToSelectSegment("painting");
      return;
    }

    state_->SetSegIDForPainting(sdw.getID());

    OmBrushPaint::PaintByClick(state_, dataClickPoint_, sdw.getID());
  }

  void mouseSelectSegment() {
    boost::optional<SegmentDataWrapper> sdw = getSelectedSegment();
    if (!sdw) {
      return;
    }

    const bool augment_selection = shiftKey_;
    return doSelectSegment(*sdw, augment_selection);
  }

  void doSelectSegment(const SegmentDataWrapper& sdw,
                       const bool augment_selection) {
    if (!sdw.IsSegmentValid()) {
      printf("not valid\n");
      return;
    }

    const OmID segmentID = sdw.getID();

    OmSegmentSelected::Set(sdw);

    OmSegmentSelector sel(sdw.MakeSegmentationDataWrapper(), this,
                          "view2dEvent");
    if (augment_selection) {
      sel.augmentSelectedSet_toggle(segmentID);
    } else {
      sel.selectJustThisSegment_toggle(segmentID);
    }
    sel.sendEvent();

    v2d_->Redraw();
  }

  void mouseShowSegmentContextMenu() {
    boost::optional<SegmentDataWrapper> sdw = getSelectedSegment();

    if (sdw) {
      segmentContextMenu_.Refresh(*sdw, state_->getViewGroupState(),
                                  dataClickPoint_);
      segmentContextMenu_.exec(event_->globalPos());
    }
  }

  boost::optional<SegmentDataWrapper> getSelectedSegment() {
    OmMipVolume* vol = state_->getVol();

    if (SEGMENTATION == vol->getVolumeType()) {
      OmSegmentation* seg = reinterpret_cast<OmSegmentation*>(vol);
      return getSelectedSegmentSegmentation(seg);
    }

    boost::optional<SegmentDataWrapper> ret;

    OmChannel* chan = reinterpret_cast<OmChannel*>(vol);
    const std::vector<OmFilter2d*> filters = chan->GetFilters();

    FOR_EACH(iter, filters) {
      OmFilter2d* filter = *iter;

      if (om::OVERLAY_SEGMENTATION == filter->FilterType()) {
        ret = getSelectedSegmentSegmentation(filter->GetSegmentation());

        if (ret) {
          break;
        }
      }
    }

    return ret;
  }

  boost::optional<SegmentDataWrapper> getSelectedSegmentSegmentation(
      OmSegmentation* segmentation) {
    const OmSegID segmentID = segmentation->GetVoxelValue(dataClickPoint_);

    if (!segmentID) {
      return boost::optional<SegmentDataWrapper>();
    }

    SegmentDataWrapper ret(segmentation, segmentID);
    return boost::optional<SegmentDataWrapper>(ret);
  }

  void fill() {
    SegmentDataWrapper sdwUnknownDepth =
        OmSegmentSelected::GetSegmentForPainting();

    SegmentDataWrapper sdw;

    if (sdwUnknownDepth.IsValidWrapper()) {
      sdw = SegmentDataWrapper(sdwUnknownDepth.FindRoot());

    } else {
      OmAskYesNoQuestion q("Are you sure you wish to fill using black?");
      if (!q.Ask()) {
        return;
      }

      sdw = SegmentDataWrapper(state_->GetSegmentationID(), 0);
    }

    OmView2dManager::AddTaskBack(
        zi::run_fn(zi::bind(&OmMouseEventPress::doFill, this, sdw)));
  }

  void doFill(const SegmentDataWrapper sdw) {
    OmFillTool fillTool(sdw, state_->getViewType());
    fillTool.Fill(dataClickPoint_);
  }

  void tellUserToSelectSegment(const QString& func) {
    OmTellInfo(
        "Please choose a color for " + func +
        " by right-clicking and selecting \n\"Set As Segment Palette Color\"");
  }

  void addAnnotation() {
    om::annotation::manager& manager =
        *state_->GetSDW().GetSegmentationPtr()->Annotations();
    OmViewGroupState* vgs = state_->getViewGroupState();

    manager.Add(dataClickPoint_, vgs->getAnnotationString(),
                vgs->getAnnotationColor(), vgs->getAnnotationSize());
  }

  void kalina() {
    boost::optional<SegmentDataWrapper> sdw = getSelectedSegment();
    if (!sdw) {
      return;
    }
    SegmentDataWrapper& seg = *sdw;
    if (!seg.IsSegmentValid()) {
      return;
    }

    SegmentationDataWrapper segmentation = seg.MakeSegmentationDataWrapper();

    if (shiftKey_) {
      // Do something different
    } else {
      // Do the same.
    }
  }
};
