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
  void reapClicked();

 protected:
  virtual void showEvent(QShowEvent* event) override;

 private:
  om::task::Dataset* dataset();
  uint32_t cellID();
  uint32_t taskID();
  void getTasks();

  QComboBox* datasetCombo_;
  QRadioButton* allCellsRadio_;
  QRadioButton* cellRadio_;
  QRadioButton* taskRadio_;
  QComboBox* cellCombo_;
  QLineEdit* taskLineEdit_;
  QPushButton* traceButton_;
  QPushButton* compareButton_;
  QPushButton* reapButton_;

  std::shared_ptr<om::task::Task> traceTask_;
  std::shared_ptr<om::task::Task> compareTask_;
  std::shared_ptr<om::task::Task> reapTask_;
  std::shared_ptr<std::vector<om::task::Dataset>> datasets_;
};
