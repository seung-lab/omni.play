#pragma once

#include "events/details/omSegmentEvent.h"
#include "events/details/omToolModeEvent.h"
#include "gui/brushToolbox/brushToolboxImpl.h"
#include "system/omStateManager.h"

class BrushToolbox : public OmToolModeEventListener,
                     public OmSegmentEventListener {
 private:
  QWidget* const parent_;
  std::unique_ptr<BrushToolboxImpl> impl_;
  OmViewGroupState* const vgs_;

  bool alreadyPoppedMenu_;

  void showForFirstTime() {
    if (alreadyPoppedMenu_) {
      return;
    }

    Show();
  }

  void SegmentModificationEvent(OmSegmentEvent*) {}
  void SegmentGUIlistEvent(OmSegmentEvent*) {}
  void SegmentSelectedEvent(OmSegmentEvent*) { showForFirstTime(); }

  void ToolModeChangeEvent() {
    if (alreadyPoppedMenu_) {
      return;
    }

    switch (OmStateManager::GetToolMode()) {
      case om::tool::ERASE:
      case om::tool::FILL:
      case om::tool::PAINT:
        Show();
        break;
      default:
        break;
    }
  }

 public:
  BrushToolbox(QWidget* parent, OmViewGroupState* vgs)
      : parent_(parent),
        impl_(new BrushToolboxImpl(parent_, vgs)),
        vgs_(vgs),
        alreadyPoppedMenu_(false) {}

  void Show() {
    impl_->show();
    alreadyPoppedMenu_ = true;
  }

  void Hide() { impl_->hide(); }
};
