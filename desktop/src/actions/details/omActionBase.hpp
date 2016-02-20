#pragma once
#include "precomp.h"

#include "system/cache/omCacheManager.h"
#include "actions/details/omUndoCommand.hpp"
#include "actions/io/omActionLogger.hpp"
#include "common/common.h"
#include "events/events.h"

template <typename IMPL>
class OmActionBase : public OmUndoCommand {
 protected:
  std::shared_ptr<IMPL> impl_;
  // these are redraw params because we only care that they center/scroll the view
  std::shared_ptr<OmSelectSegmentsParams> redrawParams_;

 public:
  OmActionBase() : impl_(std::make_shared<IMPL>()) {}

  OmActionBase(std::shared_ptr<IMPL> impl) : impl_(impl) {}

  virtual ~OmActionBase() {}

  void SetRedrawParams(std::shared_ptr<OmSelectSegmentsParams> redrawParams) {
    redrawParams_ = redrawParams;
  }

  void notifyRedrawEvent() {
    OmCacheManager::TouchFreshness();
    if (redrawParams_) {
      om::event::SegmentModified(redrawParams_);
    } else {
      om::event::Redraw2d();
      om::event::Redraw3d();
    }
  }

 protected:
  virtual void Action() {
    impl_->Execute();
    notifyRedrawEvent();
  }

  virtual void UndoAction() {
    impl_->Undo();
    notifyRedrawEvent();
  }

  virtual std::string Description() { return impl_->Description(); }

  virtual void save(const std::string& comment) {
    OmActionLogger::save(impl_, comment);
  }
};
