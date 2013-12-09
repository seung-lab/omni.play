#pragma once

/*
 * An interface for widgets belonging to the OmView3d viewing system.
 *
 * Can call a View3d event if the widget needs to be redrawn.
 */

#include <QFont>

class OmCamera;

namespace om {
namespace v3d {

class View3d;

static const float TEXT_COLOR[4] = { 1, 1, 1, 0.8 };

class Widget {
 protected:
  View3d& view3d_;
  bool enabled_;
  QFont font_;

 public:
  explicit Widget(View3d& view3d)
      : view3d_(view3d), enabled_(false), font_(QFont("Helvetica", 12)) {}

  virtual ~Widget() {}

  virtual void Draw() = 0;

  bool enabled() { return enabled_; }

  void enable(bool e) { enabled_ = e; }
};
}
}  // om::v3d::
