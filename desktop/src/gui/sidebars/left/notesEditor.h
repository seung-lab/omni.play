#pragma once

#include "precomp.h"

namespace om {
namespace task {
class Task;
}
namespace network {
namespace http {
class PutRequest;
}
}
}

class NotesEditor : public QTextEdit {
 public:
  NotesEditor(QWidget* parent);
  ~NotesEditor();

  void resetNotes(std::shared_ptr<om::task::Task> task);

 protected:
  void focusOutEvent(QFocusEvent* e) override;

  std::shared_ptr<om::network::http::PutRequest> updateNotesRequest_;
};