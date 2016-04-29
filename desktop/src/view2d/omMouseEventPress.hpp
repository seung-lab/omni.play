#pragma once

#include "precomp.h"

#include "annotation/annotation.h"
#include "actions/omActions.h"
#include "gui/widgets/omAskYesNoQuestion.hpp"
#include "gui/widgets/omSegmentContextMenu.h"
#include "gui/widgets/omTellInfo.hpp"
#include "gui/controls/controlContext.hpp"
#include "gui/controls/joiningSplittingControls.hpp"
#include "landmarks/omLandmarks.hpp"
#include "view2d/brush/omBrushSelect.hpp"
#include "view2d/omFillTool.hpp"
#include "view2d/omMouseEventUtils.hpp"
#include "view2d/omView2d.h"
#include "view2d/omView2dState.hpp"
#include "segment/omSegments.h"
#include "segment/selection.hpp"

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
    bool gButton_;
    bool tButton_;
  om::tool::mode tool_;
  QMouseEvent* event_;
  om::coords::Global dataClickPoint_;

  friend class OmMouseEventState;

 public:
  OmMouseEventPress(OmView2d* v2d, OmView2dState* state)
      : v2d_(v2d), state_(state) {}

  void Press(QMouseEvent* event) {
    setState(event);

    state_->SetMousePanStartingPt(
        om::coords::Screen(event->x(), event->y(), state_->Coords()));

    std::unique_ptr<ControlContext> context;
    switch (tool_) {
      case om::tool::JOIN:
      case om::tool::SPLIT:
      case om::tool::MULTISPLIT:
        context = std::unique_ptr<ControlContext> {
          std::make_unique<JoiningSplittingControls>(
              &state_->getViewGroupState(), 
              om::mouse::event::getSelectedSegment(
                *state_, dataClickPoint_).get_ptr(),
              tool_) };
        break;
      default:
        break;
        // fallthrough
    }

    if (context && context->mousePressEvent(event)) {
      return;
      //default camera control context
    }

    if (leftMouseButton_) {
      switch (tool_) {
        case om::tool::VALIDATE:
          om::common::SetValid setValid;
          if (controlKey_) {
            setValid = om::common::SetValid::SET_NOT_VALID;
          } else {
            setValid = om::common::SetValid::SET_VALID;
          }
          doFindandSetSegmentValid(setValid);
          return;
        case om::tool::SHATTER:
          doFindAndShatterSegment();
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
    om::coords::Screen clicked(event->x(), event->y(), state_->Coords());
    dataClickPoint_ = clicked.ToGlobal();
  }

  void doFindAndShatterSegment() {
    boost::optional<SegmentDataWrapper> sdw =
      om::mouse::event::getSelectedSegment(*state_, dataClickPoint_);

    if (!sdw) {
      return;
    }

    OmActions::ShatterSegment(sdw->MakeSegmentationDataWrapper(),
                              sdw->GetSegment());
    OmStateManager::SetOldToolModeAndSendEvent();
  }

  void doFindandSetSegmentValid(const om::common::SetValid setValid) {
    boost::optional<SegmentDataWrapper> sdw =
      om::mouse::event::getSelectedSegment(*state_, dataClickPoint_);

    // return if we couldn't find the segment, or if the segment is invalid
    // or if the segment clicked is not in our current selection
    if (!sdw || !sdw->IsSegmentValid() 
        || !sdw->Selection()->IsSegmentSelected(sdw->GetSegmentID())) {
      return;
    }

    OmActions::ValidateSegment(*sdw, setValid, true);
  }

  void doFindAndCutSegment() {
    boost::optional<SegmentDataWrapper> sdw =
      om::mouse::event::getSelectedSegment(*state_, dataClickPoint_);

    if (!sdw) {
      return;
    }

    OmActions::CutSegment(*sdw);
  }

  void mouseSetCrosshair() {
    v2d_->Redraw();
    setDepth();
    OmStateManager::SetToolModeAndSendEvent(om::tool::PAN);
  }

  void setDepth() {
    const om::coords::Screen screenc =
        om::coords::Screen(event_->x(), event_->y(), state_->Coords());
    const om::coords::Global newloc = screenc.ToGlobal();
    state_->setLocation(newloc);

    om::event::ViewCenterChanged();
  }

  void mouseLeftButton() {
    if ( controlKey_ && !shiftKey_ && !altKey_ ) {
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
        state_->getViewGroupState().Landmarks().Add(
            om::mouse::event::getSelectedSegment(*state_, dataClickPoint_),
                                                    dataClickPoint_);
        break;
      case om::tool::CUT:
        doFindAndCutSegment();
        break;
      case om::tool::ANNOTATE:
        addAnnotation();
        break;
      case om::tool::GROW:
        grow();
        break;
      default:
        return;
    }

    state_->SetLastDataPoint(dataClickPoint_);

    v2d_->Redraw();
  }

  void selectSegments() {
    om::common::AddOrSubtract addOrSubtractSegments =
        altKey_ ? om::common::AddOrSubtract::SUBTRACT
                : om::common::AddOrSubtract::ADD;

    OmBrushSelect::StartSelector(state_, dataClickPoint_,
                                 addOrSubtractSegments);
  }

  void erase() { OmBrushPaint::PaintByClick(state_, dataClickPoint_, 0); }

  void paint() {
    SegmentDataWrapper sdw = OmSegmentSelected::GetSegmentForPainting();
    if (!sdw.IsSegmentValid()) {
      tellUserToSelectSegment("painting");
      return;
    }

    state_->SetSegIDForPainting(sdw.GetID());

    OmBrushPaint::PaintByClick(state_, dataClickPoint_, sdw.GetID());
  }

  void mouseSelectSegment() {
    boost::optional<SegmentDataWrapper> sdw =
      om::mouse::event::getSelectedSegment(*state_, dataClickPoint_);
    if (!sdw) {
      return;
    }

    const bool augment_selection = shiftKey_;
    return doSelectSegment(*sdw, augment_selection);
  }

  void doSelectSegment(const SegmentDataWrapper& sdw,
                       const bool augment_selection) {
    if (!sdw.IsSegmentValid()) {
      log_infos << "not valid";
      return;
    }

    const om::common::ID segmentID = sdw.GetID();

    OmSegmentSelected::Set(sdw);

    OmSegmentSelector sel(sdw.MakeSegmentationDataWrapper(), this,
                          "view2dEvent");
    if (augment_selection) {
      sel.augmentSelectedSet_toggle(segmentID);
    } else {
      sel.selectJustThisSegment_toggle(segmentID);
    }

    v2d_->Redraw();
  }

  void mouseShowSegmentContextMenu() {
    boost::optional<SegmentDataWrapper> sdw =
      om::mouse::event::getSelectedSegment(*state_, dataClickPoint_);

    if (sdw) {
      segmentContextMenu_.Refresh(*sdw, state_->getViewGroupState(),
                                  dataClickPoint_);
      segmentContextMenu_.exec(event_->globalPos());
    }
  }


  void fill() {
    SegmentDataWrapper sdwUnknownDepth =
        OmSegmentSelected::GetSegmentForPainting();

    SegmentDataWrapper sdw;

    if (sdwUnknownDepth.IsValidWrapper()) {
      sdw = sdwUnknownDepth.FindRootSDW();

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
    OmFillTool fillTool(sdw);
    fillTool.Fill(dataClickPoint_);
  }

  void tellUserToSelectSegment(const QString& func) {
    OmTellInfo(
        "Please choose a color for " + func +
        " by right-clicking and selecting \n\"Set As Segment Palette Color\"");
  }

  void addAnnotation() {
    auto& manager = state_->GetSDW().GetSegmentation()->Annotations();

    OmViewGroupState& vgs = state_->getViewGroupState();

    manager.Add(dataClickPoint_, vgs.getAnnotationString(),
                vgs.getAnnotationColor(), vgs.getAnnotationSize());
  }

  void grow() {
    boost::optional<SegmentDataWrapper> sdw =
      om::mouse::event::getSelectedSegment(*state_, dataClickPoint_);
    if(!sdw){
      return;
    }
    SegmentDataWrapper& seg = *sdw;

    if(!seg.IsSegmentValid()) {
      return;
    }
    SegmentationDataWrapper segmentation = seg.MakeSegmentationDataWrapper();

    OmSegments *Segments = segmentation.Segments();

    OmSegmentSelector sel(segmentation, this, "view2dEvent" );

    // only call grow when shift key is pressed
    if (shiftKey_) {
      if (altKey_) {
        Segments->Trim(&sel, seg.GetSegmentID());
      } else {
        Segments->AddSegments_BreadthFirstSearch(&sel, seg.GetSegmentID());
      }
    }
  }
};
