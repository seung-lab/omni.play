#pragma once
#include <QtGui>
#include "events/listeners.h"

class TaskInfoWidget : public QWidget, public om::event::TaskEventListener {
  Q_OBJECT;

 public:
  TaskInfoWidget(QWidget* parent);
  ~TaskInfoWidget();

  void TaskChangeEvent();

 public
Q_SLOTS:
  void doneButtonPressed();
  void taskSelectorButtonPressed();

 private:
  void updateInfo();
  QLabel* idLabel_;
  QLabel* cellIdLabel_;
  QPushButton* doneButton_;
  QPushButton* taskSelectorButton_;
};