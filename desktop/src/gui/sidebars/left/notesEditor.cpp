#include "gui/sidebars/left/notesEditor.h"
#include "task/task.h"
#include "task/taskManager.h"
#include "gui/exec.hpp"

using namespace om::task;

NotesEditor::NotesEditor(QWidget* parent) : QTextEdit(parent) {}
NotesEditor::~NotesEditor() {
  if (updateNotesRequest_) {
    updateNotesRequest_->Detach();
  }
}

void NotesEditor::resetNotes(std::shared_ptr<Task> task) {
  setText(task ? QString::fromStdString(task->Notes()) : tr(""));
}

void NotesEditor::focusOutEvent(QFocusEvent* e) {
  QTextEdit::focusOutEvent(e);
  auto task = TaskManager::currentTask();
  if (!task) {
    return;
  }
  updateNotesRequest_ =
      TaskManager::UpdateNotes(task->Id(), toPlainText().toStdString());
  if (!updateNotesRequest_) {
    resetNotes(task);
    return;
  }

  *updateNotesRequest_ >>= [this, task]() {
    if (updateNotesRequest_->returnCode() < 200 ||
        300 <= updateNotesRequest_->returnCode()) {
      resetNotes(task);
    }
  };
}
