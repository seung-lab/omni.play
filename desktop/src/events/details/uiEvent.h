#pragma once

#include <QWidget>
#include "events/details/event.h"

namespace om {
namespace event {

class UIEvent : public Event {
 public:
  static const QEvent::Type UPDATE_SEGMENT_PROP_WIDGET;

  explicit UIEvent(QEvent::Type type) : Event(Klass::ui, type) {}

  UIEvent(QWidget* widget, const QString& title)
      : Event(Klass::ui, UPDATE_SEGMENT_PROP_WIDGET),
        widget_(widget),
        title_(title) {}

  void Dispatch(Listener* base) {
    auto* list = dynamic_cast<UIEventListener*>(base);
    assert(list);

    if (type_ == UPDATE_SEGMENT_PROP_WIDGET)
      return list->UpdateSegmentPropWidgetEvent(this);
    throw om::ArgException("unknown event type");
  }

  QWidget* Widget() { return widget_; }

  const QString& Title() { return title_; }

 private:
  QWidget* widget_;
  QString title_;
};
}
}  // om::event::
