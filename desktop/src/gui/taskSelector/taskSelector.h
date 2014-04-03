#pragma once
#include "precomp.h"

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
  void updateList();
  void onManualEntry();
  void updateCells();
  void traceClicked();
  void compareClicked();
  void completedChanged();
  virtual void showEvent(QShowEvent* event = nullptr) override;

 protected:
  virtual void accept() override;

  virtual QSize sizeHint() const override;

 private:
  uint8_t datasetID();
  om::task::Dataset* dataset();
  uint32_t cellID();
  uint32_t taskID();
  int selectedTaskId();
  void getTasks();

  int oldCellSelection_;

  QComboBox* datasetCombo_;
  QComboBox* cellCombo_;
  QLineEdit* taskLineEdit_;
  QTableWidget* taskTable_;
  QPushButton* traceButton_;
  QPushButton* compareButton_;
  QPushButton* closeButton_;
  QPushButton* refreshButton_;
  QCheckBox* completedTasksCheckbox_;

  std::shared_ptr<om::task::Task> traceTask_;
  std::shared_ptr<om::task::Task> compareTask_;
  std::shared_ptr<std::vector<om::task::Dataset>> datasets_;
};
