#pragma once
#include <QtGui>
#include "events/listeners.h"

namespace om {
namespace task {
struct SegGroup;
}
}

class TaskInfoWidget : public QWidget, public om::event::TaskEventListener {
  Q_OBJECT;

 public:
  TaskInfoWidget(QWidget* parent);
  ~TaskInfoWidget();

  void TaskStartedEvent();

 public
Q_SLOTS:
  void doneButtonPressed();
  void taskSelectorButtonPressed();

 private:
  void updateInfo();
  size_t getSize(const om::task::SegGroup&) const;
  QLabel* idLabel_;
  QLabel* cellIdLabel_;
  QPushButton* doneButton_;
  QPushButton* taskSelectorButton_;
  QFrame* buttons_;
};