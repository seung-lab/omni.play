#include "gui/taskSelector/taskSelector.h"
#include "system/omLocalPreferences.hpp"

using namespace om::task;

TaskSelector::TaskSelector(QWidget* p) : QDialog(p), populating_(false) {
  QGridLayout* layout = new QGridLayout(this);

  datasetCombo_ = new QComboBox(this);
  layout->addWidget(datasetCombo_, 0, 0, 1, 2);
  om::connect(datasetCombo_, SIGNAL(currentIndexChanged(int)), this,
              SLOT(updateCells()));

  cellCombo_ = new QComboBox(this);
  layout->addWidget(cellCombo_, 0, 2, 1, 2);
  om::connect(cellCombo_, SIGNAL(currentIndexChanged(int)), this,
              SLOT(updateEnabled()));

  taskLineEdit_ = new QLineEdit(this);
  layout->addWidget(taskLineEdit_, 0, 5, 1, 1);
  om::connect(taskLineEdit_, SIGNAL(textEdited(const QString&)), this,
              SLOT(updateEnabled()));

  taskTable_ = new QTableWidget(this);
  taskTable_->setRowCount(10);
  QStringList headerLabels;
  headerLabels << "Id"
               << "Cell"
               << "Weight"
               << "Comparison"
               << "Path";
  taskTable_->setColumnCount(headerLabels.size());
  taskTable_->setHorizontalHeaderLabels(headerLabels);

  layout->addWidget(taskTable_, 1, 0, 1, 6);

  traceButton_ = new QPushButton(tr("Trace"), this);
  layout->addWidget(traceButton_, 2, 0);
  om::connect(traceButton_, SIGNAL(clicked()), this, SLOT(traceClicked()));

  compareButton_ = new QPushButton(tr("Compare"), this);
  layout->addWidget(compareButton_, 2, 1);
  om::connect(compareButton_, SIGNAL(clicked()), this, SLOT(compareClicked()));

  closeButton_ = new QPushButton(tr("Close"), this);
  layout->addWidget(closeButton_, 2, 5);
  om::connect(closeButton_, SIGNAL(clicked()), this, SLOT(reject()));

  traceButton_->setDefault(true);
  setWindowTitle(tr("Task Selector"));
}

void TaskSelector::showEvent(QShowEvent* event) {
  populating_ = true;
  datasets_ = TaskManager::GetDatasets();

  datasetCombo_->clear();
  if ((bool)datasets_) {
    int prefDataset = OmLocalPreferences::getDataset();
    for (int i = 0; i < datasets_->size(); ++i) {
      Dataset& ds = (*datasets_)[i];
      ds.LoadCells();
      datasetCombo_->addItem(
          QString::fromStdString(std::to_string(ds.id()) + ". " + ds.name()),
          i);
      if (ds.id() == prefDataset) {
        datasetCombo_->setCurrentIndex(i);
      }
    }
  }
  updateCells();
  populating_ = false;
}

void TaskSelector::updateCells() {
  if (populating_) {
    return;
  }

  cellCombo_->clear();
  cellCombo_->addItem("", 0);
  if (dataset()) {
    auto cells = dataset()->cells();
    if (cells) {
      for (int i = 0; i < cells->size(); ++i) {
        Cell& c = (*cells)[i];
        cellCombo_->addItem(
            QString::fromStdString(std::to_string(c.CellID) + " - " + c.Name),
            i + 1);
      }
    }
  }
  updateEnabled();
}

void TaskSelector::updateEnabled() { getTasks(); }

void TaskSelector::traceClicked() {
  TaskManager::LoadTask(traceTask_);
  accept();
}
void TaskSelector::compareClicked() {
  TaskManager::LoadTask(compareTask_);
  accept();
}

uint8_t TaskSelector::datasetID() {
  return datasetCombo_->itemData(datasetCombo_->currentIndex()).toInt();
}

Dataset* TaskSelector::dataset() {
  if (!datasets_) {
    return nullptr;
  }
  return &(*datasets_)[datasetID()];
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
  auto idx = cellCombo_->itemData(cellCombo_->currentIndex()).toInt();
  if (!idx) {
    return 0;
  }
  if (idx > cellsref.size()) {
    return 0;
  }
  return cellsref[idx - 1].CellID;
  return 0;
}

uint32_t TaskSelector::taskID() { return taskLineEdit_->text().toInt(); }

template <typename T>
QTableWidgetItem* makeTableItem(const T val) {
  QTableWidgetItem* item = new QTableWidgetItem();
  item->setData(0, val);
  return item;
}

void TaskSelector::getTasks() {
  auto dsid = datasetID();
  auto cid = cellID();
  auto tasks = TaskManager::GetTasks(dsid, cid, 3);
  for (size_t i = 0; i < std::min((size_t)10, tasks->size()); ++i) {
    auto& t = (*tasks)[i];

    taskTable_->setItem(i, 0, makeTableItem(t.id));
    taskTable_->setItem(i, 1, makeTableItem(t.cell));
    taskTable_->setItem(i, 2, makeTableItem(t.weight));
    taskTable_->setItem(i, 3, makeTableItem(t.inspected_weight == t.weight));
    taskTable_->setItem(i, 4, makeTableItem(QString::fromStdString(t.path)));
  }
  taskTable_->resizeColumnsToContents();
}

void TaskSelector::accept() {
  int index = datasetCombo_->itemData(datasetCombo_->currentIndex()).toInt();
  OmLocalPreferences::setDataset((*datasets_)[index].id());
  QDialog::accept();
}
