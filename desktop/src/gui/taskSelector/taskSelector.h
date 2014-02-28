#pragma once

#include <QtGui>

#include "gui/widgets/omTellInfo.hpp"
#include "system/account.h"
#include "system/omConnect.hpp"
#include "task/taskManager.h"
#include "task/tracingTask.h"
#include "network/http/http.hpp"

namespace om {
namespace task {
class Dataset;
}
}

class TaskSelector : public QDialog {
  Q_OBJECT;

 public:
  TaskSelector(QWidget* p = nullptr);

 public
Q_SLOTS:
  void updateEnabled();
  void updateCells();
  void traceClicked();
  void compareClicked();

 protected:
  virtual void showEvent(QShowEvent* event) override;
  virtual void accept() override;

 private:
  uint8_t datasetID();
  om::task::Dataset* dataset();
  uint32_t cellID();
  uint32_t taskID();
  void getTasks();

  bool populating_;

  QComboBox* datasetCombo_;
  QComboBox* cellCombo_;
  QLineEdit* taskLineEdit_;
  QTableWidget* taskTable_;
  QPushButton* traceButton_;
  QPushButton* compareButton_;
  QPushButton* closeButton_;

  std::shared_ptr<om::task::Task> traceTask_;
  std::shared_ptr<om::task::Task> compareTask_;
  std::shared_ptr<std::vector<om::task::Dataset>> datasets_;
};
