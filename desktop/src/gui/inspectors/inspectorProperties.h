#pragma once

#include "events/listeners.h"

#include <QtGui>
#include <QWidget>

class OmViewGroupState;

class InspectorProperties : public QDialog,
                            public om::event::UIEventListener,
                            public om::event::SegmentEventListener {
  Q_OBJECT;
  ;
 public:
  InspectorProperties(QWidget* parent, OmViewGroupState* vgs);

  virtual ~InspectorProperties();

  void SetOrReplaceWidget(QWidget* incomingWidget, const QString& title);

  OmViewGroupState* GetViewGroupState() { return vgs_; }

 public
Q_SLOTS:
  void CloseDialog();

 private:
  OmViewGroupState* vgs_;

  std::unique_ptr<QWidget> widget_;

  QVBoxLayout* mainLayout_;

  void UpdateSegmentPropWidgetEvent(om::event::UIEvent*);
  void SegmentGUIlistEvent(om::event::SegmentEvent*) {}
  void SegmentSelectedEvent(om::event::SegmentEvent*) {}
  void SegmentModificationEvent(om::event::SegmentEvent* event);
};
