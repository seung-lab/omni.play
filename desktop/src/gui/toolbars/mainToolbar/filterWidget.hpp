#pragma once
#include "precomp.h"

#include "gui/toolbars/mainToolbar/filterWidgetImpl.hpp"
#include "zi/omUtility.h"

class FilterWidget : private om::singletonBase<FilterWidget> {
 private:
  FilterWidgetImpl* impl_;

 public:
  static QWidget* Widget() { return instance().impl_; }

  static void Create() { instance().impl_ = new FilterWidgetImpl(); }

  static void Cycle() { instance().impl_->Cycle(); }

  static void IncreaseAlpha() { instance().impl_->IncreaseAlpha(); }

  static void DecreaseAlpha() { instance().impl_->DecreaseAlpha(); }

 private:
  FilterWidget() : impl_(nullptr) {}
  ~FilterWidget() {}

  friend class zi::singleton<FilterWidget>;
};
