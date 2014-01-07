#pragma once
#include <QtGui>
#include "events/listeners.h"

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
  size_t getSize(const std::pair<std::string, const om::common::SegIDSet>&)
      const;
  QLabel* idLabel_;
  QLabel* cellIdLabel_;
  QPushButton* doneButton_;
  QPushButton* taskSelectorButton_;
  QFrame* buttons_;
};