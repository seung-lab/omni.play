#pragma once
#include "precomp.h"
#include "events/listeners.h"

namespace om {
namespace task {
class Task;
struct SegGroup;
}
}

class TaskInfoWidget : public QWidget, public om::event::TaskEventListener {
  Q_OBJECT;

 public:
  TaskInfoWidget(QWidget* parent);
  ~TaskInfoWidget();

  void TaskStartedEvent() override;
  void TaskChangeEvent() override;

 public
Q_SLOTS:
  void doneButtonPressed();
  void taskSelectorButtonPressed();
  void notesTextChanged();

 private:
  void updateInfo();
  void resetNotes(std::shared_ptr<om::task::Task> task);
  size_t getSize(const om::task::SegGroup&) const;
  QLabel* idLabel_;
  QLabel* cellIdLabel_;
  QTextEdit* notesField_;
  QPushButton* doneButton_;
  QPushButton* taskSelectorButton_;
  QScrollArea* scrollable_;
  QFrame* buttons_;
};