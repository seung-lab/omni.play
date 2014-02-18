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
  virtual void accept() override;

 private:
  int datasetID() const;
  uint32_t cellID() const;
  uint32_t taskID() const;
  void getTasks();
  std::shared_ptr<om::task::Task> traceTask() const;
  std::shared_ptr<om::task::Task> compareTask() const;
  std::shared_ptr<om::task::Task> reapTask() const;

  QComboBox* datasetCombo_;
  QRadioButton* allCellsRadio_;
  QRadioButton* cellRadio_;
  QRadioButton* taskRadio_;
  QComboBox* cellCombo_;
  QLineEdit* taskLineEdit_;
  QPushButton* traceButton_;
  QPushButton* compareButton_;
  QPushButton* reapButton_;

  std::shared_ptr<om::task::Task> traceTaskAssigned_;
  std::shared_ptr<om::task::Task> compareTaskAssigned_;

  std::shared_ptr<om::task::Task> traceTaskByID_;
  std::shared_ptr<om::task::Task> compareTaskByID_;
  std::shared_ptr<om::task::Task> reapTaskByID_;

  std::shared_ptr<om::task::Datasets> datasets_;
  std::map<int, std::shared_ptr<om::task::Cells>> cells_;
};
