#pragma once
#include "precomp.h"

#include "events/listeners.h"
#include "gui/brushToolbox/brushToolboxImpl.h"
#include "system/omStateManager.h"
#include "gui/tools.hpp"

class BrushToolbox : public om::event::ToolModeEventListener,
                     public om::event::SegmentEventListener {
 private:
  QWidget* const parent_;
  std::unique_ptr<BrushToolboxImpl> impl_;

  OmViewGroupState& vgs_;

  bool alreadyPoppedMenu_;

  void showForFirstTime() {
    if (alreadyPoppedMenu_) {
      return;
    }

    Show();
  }

  void SegmentModificationEvent(om::event::SegmentEvent*) {}
  void SegmentGUIlistEvent(om::event::SegmentEvent*) {}
  void SegmentSelectedEvent(om::event::SegmentEvent*) { showForFirstTime(); }

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
  BrushToolbox(QWidget* parent, OmViewGroupState& vgs)
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
