#include "gui/taskSelector/taskSelector.h"

using namespace om::task;

TaskSelector::TaskSelector(QWidget* p) : QDialog(p) {
  QGridLayout* layout = new QGridLayout(this);

  allCellsRadio_ = new QRadioButton(tr("All Cells"), this);
  allCellsRadio_->setEnabled(false);
  layout->addWidget(allCellsRadio_, 0, 0, 1, 3);
  om::connect(allCellsRadio_, SIGNAL(toggled(bool)), this,
              SLOT(updateEnabled()));

  cellRadio_ = new QRadioButton(tr("Specific Cell"), this);
  layout->addWidget(cellRadio_, 1, 0, 1, 2);
  om::connect(cellRadio_, SIGNAL(toggled(bool)), this, SLOT(updateEnabled()));
  cellCombo_ = new QComboBox(this);
  layout->addWidget(cellCombo_, 1, 2);
  om::connect(cellCombo_, SIGNAL(currentIndexChanged(int)), this,
              SLOT(updateEnabled()));

  taskRadio_ = new QRadioButton(tr("Specific Task"), this);
  layout->addWidget(taskRadio_, 2, 0, 1, 2);
  om::connect(taskRadio_, SIGNAL(toggled(bool)), this, SLOT(updateEnabled()));
  taskLineEdit_ = new QLineEdit(this);
  layout->addWidget(taskLineEdit_, 2, 2);
  om::connect(taskLineEdit_, SIGNAL(returnPressed()), this,
              SLOT(updateEnabled()));

  QHBoxLayout* buttonsLayout = new QHBoxLayout(this);
  layout->addLayout(buttonsLayout, 3, 0, 1, 3);

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

void TaskSelector::updateEnabled() {
  getTasks();
  cellCombo_->setEnabled(cellRadio_->isChecked() && cellCombo_->count() > 0);
  taskLineEdit_->setEnabled(taskRadio_->isChecked());
  traceButton_->setEnabled((bool)traceTask_);
  compareButton_->setEnabled((bool)compareTask_);
  reapButton_->setEnabled((bool)reapTask_);
}

void TaskSelector::showEvent(QShowEvent* event) {
  cells_ = TaskManager::GetCells();
  cellCombo_->clear();
  if ((bool)cells_) {
    for (int i = 0; i < cells_->size(); ++i) {
      Cell& c = (*cells_)[i];
      cellCombo_->addItem(
          QString::fromStdString(std::to_string(c.CellID) + " - " + c.Name), i);
    }
  }
  updateEnabled();
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

uint32_t TaskSelector::cellID() {
  if (!cells_) {
    return 0;
  }
  if (cellRadio_->isChecked()) {
    return (*cells_)[cellCombo_->itemData(cellCombo_->currentIndex()).toInt()]
        .CellID;
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