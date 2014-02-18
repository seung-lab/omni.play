#include "gui/taskSelector/taskSelector.h"
#include "system/omLocalPreferences.hpp"

using namespace om::task;

TaskSelector::TaskSelector(QWidget* p) : QDialog(p) {
  QGridLayout* layout = new QGridLayout(this);

  datasetCombo_ = new QComboBox(this);
  layout->addWidget(datasetCombo_, 0, 0, 1, 3);

  allCellsRadio_ = new QRadioButton(tr("All Cells"), this);
  allCellsRadio_->setEnabled(false);
  layout->addWidget(allCellsRadio_, 1, 0, 1, 3);

  cellRadio_ = new QRadioButton(tr("Specific Cell"), this);
  layout->addWidget(cellRadio_, 2, 0, 1, 2);

  cellCombo_ = new QComboBox(this);
  layout->addWidget(cellCombo_, 2, 2);

  taskRadio_ = new QRadioButton(tr("Specific Task"), this);
  layout->addWidget(taskRadio_, 3, 0, 1, 2);
  taskLineEdit_ = new QLineEdit(this);
  layout->addWidget(taskLineEdit_, 3, 2);

  QHBoxLayout* buttonsLayout = new QHBoxLayout(this);
  layout->addLayout(buttonsLayout, 4, 0, 1, 3);

  traceButton_ = new QPushButton(tr("Trace"), this);
  buttonsLayout->addWidget(traceButton_);
  compareButton_ = new QPushButton(tr("Compare"), this);
  buttonsLayout->addWidget(compareButton_);
  reapButton_ = new QPushButton(tr("Reap"), this);
  buttonsLayout->addWidget(reapButton_);

  cellRadio_->setChecked(true);
  traceButton_->setDefault(true);
  setWindowTitle(tr("Task Selector"));
}

void TaskSelector::showEvent(QShowEvent* event) {
  datasets_ = TaskManager::GetDatasets();

  datasetCombo_->clear();
  if ((bool)datasets_) {
    int prefDataset = OmLocalPreferences::getDataset();
    for (int i = 0; i < datasets_->size(); ++i) {
      Dataset& ds = (*datasets_)[i];
      datasetCombo_->addItem(
          QString::fromStdString(std::to_string(ds.id) + ". " + ds.name), i);
      if (ds.id == prefDataset) {
        datasetCombo_->setCurrentIndex(i);
      }
    }
  }
  updateCells();
  om::connect(datasetCombo_, SIGNAL(currentIndexChanged(int)), this,
              SLOT(updateCells()));
  om::connect(allCellsRadio_, SIGNAL(toggled(bool)), this,
              SLOT(updateEnabled()));
  om::connect(cellRadio_, SIGNAL(toggled(bool)), this, SLOT(updateEnabled()));
  om::connect(cellCombo_, SIGNAL(currentIndexChanged(int)), this,
              SLOT(updateEnabled()));
  om::connect(taskRadio_, SIGNAL(toggled(bool)), this, SLOT(updateEnabled()));
  om::connect(taskLineEdit_, SIGNAL(textEdited(const QString&)), this,
              SLOT(updateEnabled()));
  om::connect(traceButton_, SIGNAL(clicked()), this, SLOT(traceClicked()));
  om::connect(compareButton_, SIGNAL(clicked()), this, SLOT(compareClicked()));
  om::connect(reapButton_, SIGNAL(clicked()), this, SLOT(reapClicked()));
}

void TaskSelector::updateCells() {
  cellCombo_->clear();
  auto dsid = datasetID();
  if (dsid) {
    return;
  }

  cells_[dsid] = TaskManager::GetCells(dsid);
  if (!cells_[dsid]) {
    return;
  }
  auto cells = cells_[dsid];

  for (int i = 0; i < cells->size(); ++i) {
    Cell& c = (*cells)[i];
    cellCombo_->addItem(
        QString::fromStdString(std::to_string(c.CellID) + " - " + c.Name), i);
  }
  updateEnabled();
}

void TaskSelector::updateEnabled() {
  getTasks();
  cellRadio_->setEnabled(cellCombo_->count() > 0);
  cellCombo_->setEnabled(cellRadio_->isChecked() && cellCombo_->count() > 0);
  taskLineEdit_->setEnabled(taskRadio_->isChecked());
  traceButton_->setEnabled((bool)traceTask());
  compareButton_->setEnabled((bool)compareTask());
  reapButton_->setEnabled((bool)reapTask());
}

std::shared_ptr<om::task::Task> TaskSelector::traceTask() const {
  return cellRadio_->isChecked() ? traceTaskAssigned_ : traceTaskByID_;
}
std::shared_ptr<om::task::Task> TaskSelector::compareTask() const {
  return cellRadio_->isChecked() ? compareTaskAssigned_ : compareTaskByID_;
}
std::shared_ptr<om::task::Task> TaskSelector::reapTask() const {
  return reapTaskByID_;
}

void TaskSelector::traceClicked() {
  TaskManager::LoadTask(traceTask());
  accept();
}
void TaskSelector::compareClicked() {
  TaskManager::LoadTask(compareTask());
  accept();
}
void TaskSelector::reapClicked() {
  TaskManager::LoadTask(reapTask());
  accept();
}

int TaskSelector::datasetID() const {
  if (!datasets_) {
    return 0;
  }
  return datasetCombo_->itemData(datasetCombo_->currentIndex()).toInt();
}

uint32_t TaskSelector::cellID() const {
  if (!cellRadio_->isChecked()) {
    return 0;
  }

  auto dsid = datasetID();
  if (!dsid) {
    return 0;
  }

  auto iter = cells_.find(dsid);
  if (iter == cells_.end()) {
    return 0;
  }

  auto& cellsref = *iter->second;
  auto idx = cellCombo_->itemData(cellCombo_->currentIndex()).toInt();
  if (idx >= cellsref.size()) {
    return 0;
  }

  return cellsref[idx].CellID;
}

uint32_t TaskSelector::taskID() const { return taskLineEdit_->text().toInt(); }

void TaskSelector::getTasks() {
  if (taskRadio_->isChecked()) {
    traceTaskByID_ = TaskManager::GetTaskByID(taskID());
    compareTaskByID_ = TaskManager::GetComparisonTaskByID(taskID());
    reapTaskByID_ = TaskManager::GetReapTask(taskID());
  } else {
    traceTaskAssigned_ = TaskManager::GetTask(cellID());
    compareTaskAssigned_ = TaskManager::GetComparisonTask(cellID());
  }
}

void TaskSelector::accept() {
  int index = datasetCombo_->itemData(datasetCombo_->currentIndex()).toInt();
  OmLocalPreferences::setDataset((*datasets_)[index].id);
  QDialog::accept();
}
