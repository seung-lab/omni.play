#include "gui/taskSelector/taskSelector.h"

using namespace om::task;

TaskSelector::TaskSelector(QWidget* p) : QDialog(p) {
  QGridLayout* layout = new QGridLayout(this);

  datasetCombo_ = new QComboBox(this);
  layout->addWidget(datasetCombo_, 0, 0, 1, 3);
  om::connect(datasetCombo_, SIGNAL(currentIndexChanged(int)), this,
              SLOT(updateCells()));

  allCellsRadio_ = new QRadioButton(tr("All Cells"), this);
  allCellsRadio_->setEnabled(false);
  layout->addWidget(allCellsRadio_, 1, 0, 1, 3);
  om::connect(allCellsRadio_, SIGNAL(toggled(bool)), this,
              SLOT(updateEnabled()));

  cellRadio_ = new QRadioButton(tr("Specific Cell"), this);
  layout->addWidget(cellRadio_, 2, 0, 1, 2);
  om::connect(cellRadio_, SIGNAL(toggled(bool)), this, SLOT(updateEnabled()));
  cellCombo_ = new QComboBox(this);
  layout->addWidget(cellCombo_, 2, 2);
  om::connect(cellCombo_, SIGNAL(currentIndexChanged(int)), this,
              SLOT(updateEnabled()));

  taskRadio_ = new QRadioButton(tr("Specific Task"), this);
  layout->addWidget(taskRadio_, 3, 0, 1, 2);
  om::connect(taskRadio_, SIGNAL(toggled(bool)), this, SLOT(updateEnabled()));
  taskLineEdit_ = new QLineEdit(this);
  layout->addWidget(taskLineEdit_, 3, 2);
  om::connect(taskLineEdit_, SIGNAL(returnPressed()), this,
              SLOT(updateEnabled()));

  QHBoxLayout* buttonsLayout = new QHBoxLayout(this);
  layout->addLayout(buttonsLayout, 4, 0, 1, 3);

  traceButton_ = new QPushButton(tr("Trace"), this);
  buttonsLayout->addWidget(traceButton_);
  om::connect(traceButton_, SIGNAL(clicked()), this, SLOT(traceClicked()));
  compareButton_ = new QPushButton(tr("Compare"), this);
  buttonsLayout->addWidget(compareButton_);
  om::connect(compareButton_, SIGNAL(clicked()), this, SLOT(compareClicked()));
  reapButton_ = new QPushButton(tr("Reap"), this);
  buttonsLayout->addWidget(reapButton_);
  om::connect(reapButton_, SIGNAL(clicked()), this, SLOT(reapClicked()));

  cellRadio_->setChecked(true);
  traceButton_->setDefault(true);
  setWindowTitle(tr("Task Selector"));
}

void TaskSelector::showEvent(QShowEvent* event) {
  datasets_ = TaskManager::GetDatasets();

  datasetCombo_->clear();
  if ((bool)datasets_) {
    for (int i = 0; i < datasets_->size(); ++i) {
      Dataset& ds = (*datasets_)[i];
      ds.LoadCells();
      datasetCombo_->addItem(
          QString::fromStdString(std::to_string(ds.id()) + ". " + ds.name()),
          i);
    }
  }
  updateCells();
}

void TaskSelector::updateCells() {
  cellCombo_->clear();
  if (dataset()) {
    auto cells = dataset()->cells();
    if (cells) {
      for (int i = 0; i < cells->size(); ++i) {
        Cell& c = (*cells)[i];
        cellCombo_->addItem(
            QString::fromStdString(std::to_string(c.CellID) + " - " + c.Name),
            i);
      }
    }
  }
  updateEnabled();
}

void TaskSelector::updateEnabled() {
  getTasks();
  cellCombo_->setEnabled(cellRadio_->isChecked() && cellCombo_->count() > 0);
  taskLineEdit_->setEnabled(taskRadio_->isChecked());
  traceButton_->setEnabled((bool)traceTask_);
  compareButton_->setEnabled((bool)compareTask_);
  reapButton_->setEnabled((bool)reapTask_);
}

void TaskSelector::traceClicked() {
  TaskManager::LoadTask(traceTask_);
  accept();
}
void TaskSelector::compareClicked() {
  TaskManager::LoadTask(compareTask_);
  accept();
}
void TaskSelector::reapClicked() {
  TaskManager::LoadTask(reapTask_);
  accept();
}

Dataset* TaskSelector::dataset() {
  if (!datasets_) {
    return nullptr;
  }
  return &(*datasets_)
              [datasetCombo_->itemData(datasetCombo_->currentIndex()).toInt()];
}

uint32_t TaskSelector::cellID() {
  if (!dataset()) {
    return 0;
  }
  auto cells = dataset()->cells();
  if (!cells) {
    return 0;
  }
  auto cellsref = *cells;
  if (cellRadio_->isChecked()) {
    auto idx = cellCombo_->itemData(cellCombo_->currentIndex()).toInt();
    if (idx || idx >= cellsref.size()) {
      return 0;
    }
    return cellsref[idx].CellID;
  }
  return 0;
}

uint32_t TaskSelector::taskID() { return taskLineEdit_->text().toInt(); }

void TaskSelector::getTasks() {
  if (taskRadio_->isChecked()) {
    traceTask_ = TaskManager::GetTaskByID(taskID());
    compareTask_ = TaskManager::GetComparisonTaskByID(taskID());
    reapTask_ = TaskManager::GetReapTask(taskID());
  } else {
    traceTask_ = TaskManager::GetTask(cellID());
    compareTask_ = TaskManager::GetComparisonTask(cellID());
    reapTask_ = std::shared_ptr<Task>();
  }
}