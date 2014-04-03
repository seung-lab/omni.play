#include "gui/taskSelector/taskSelector.h"
#include "system/omLocalPreferences.hpp"

using namespace om::task;

TaskSelector::TaskSelector(QWidget* p) : QDialog(p) {
  QGridLayout* layout = new QGridLayout(this);

  datasetCombo_ = new QComboBox(this);
  layout->addWidget(datasetCombo_, 0, 0, 1, 2);
  om::connect(datasetCombo_, SIGNAL(currentIndexChanged(int)), this,
              SLOT(updateCells()));

  cellCombo_ = new QComboBox(this);
  layout->addWidget(cellCombo_, 0, 2, 1, 2);
  om::connect(cellCombo_, SIGNAL(currentIndexChanged(int)), this,
              SLOT(updateList()));

  taskLineEdit_ = new QLineEdit(this);
  layout->addWidget(taskLineEdit_, 0, 4);
  om::connect(taskLineEdit_, SIGNAL(textEdited(const QString&)), this,
              SLOT(onManualEntry()));

  refreshButton_ = new QPushButton(tr("Refresh"), this);
  layout->addWidget(refreshButton_, 0, 5);
  om::connect(refreshButton_, SIGNAL(clicked()), this, SLOT(showEvent()));

  taskTable_ = new QTableWidget(this);
  taskTable_->setRowCount(0);

  QStringList headerLabels;
  headerLabels << "Id"
               << "Cell"
               << "Parent"
               << "Weight"
               << "%"
               << "Path"
               << "Users"
               << "Notes";
  taskTable_->setColumnCount(headerLabels.size());
  taskTable_->setHorizontalHeaderLabels(headerLabels);

  taskTable_->setSortingEnabled(true);
  taskTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
  taskTable_->setSelectionMode(QAbstractItemView::SingleSelection);
  taskTable_->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  taskTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);

  layout->addWidget(taskTable_, 1, 0, 1, 6);
  om::connect(taskTable_, SIGNAL(itemSelectionChanged()), this,
              SLOT(updateEnabled()));

  traceButton_ = new QPushButton(tr("Trace"), this);
  layout->addWidget(traceButton_, 2, 0);
  om::connect(traceButton_, SIGNAL(clicked()), this, SLOT(traceClicked()));

  compareButton_ = new QPushButton(tr("Compare"), this);
  layout->addWidget(compareButton_, 2, 1);
  om::connect(compareButton_, SIGNAL(clicked()), this, SLOT(compareClicked()));

  completedTasksCheckbox_ = new QCheckBox(tr("Show Completed"), this);
  layout->addWidget(completedTasksCheckbox_, 2, 2);
  om::connect(completedTasksCheckbox_, SIGNAL(stateChanged(int)), this,
              SLOT(completedChanged()));

  closeButton_ = new QPushButton(tr("Close"), this);
  layout->addWidget(closeButton_, 2, 5);
  om::connect(closeButton_, SIGNAL(clicked()), this, SLOT(reject()));

  traceButton_->setDefault(true);
  setWindowTitle(tr("Task Selector"));

  for (int i = 0; i < layout->columnCount(); ++i) {
    layout->setColumnMinimumWidth(i,
                                  sizeHint().width() / layout->columnCount());
  }
}

QSize TaskSelector::sizeHint() const { return QSize(900, 600); }

void TaskSelector::showEvent(QShowEvent* event) {
  oldCellSelection_ = cellID();
  auto oldDatasetSelection = datasetID();

  TaskManager::Refresh();
  datasets_ = TaskManager::GetDatasets();

  datasetCombo_->clear();
  if ((bool)datasets_) {
    if (!oldDatasetSelection) {
      oldDatasetSelection = OmLocalPreferences::getDataset();
    }
    for (int i = 0; i < datasets_->size(); ++i) {
      Dataset& ds = (*datasets_)[i];
      ds.LoadCells();
      datasetCombo_->addItem(
          QString::fromStdString(std::to_string(ds.id()) + ". " + ds.name()),
          ds.id());
      if (ds.id() == oldDatasetSelection) {
        datasetCombo_->setCurrentIndex(i);
      }
    }
  }
}

void TaskSelector::updateCells() {
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
        if (c.CellID == oldCellSelection_) {
          cellCombo_->setCurrentIndex(i + 1);
        }
      }
    }
  }
}

void TaskSelector::updateList() {
  getTasks();
  updateEnabled();
}

void TaskSelector::updateEnabled() {
  auto row = taskTable_->currentRow();
  int col = 0;  // colume 0 is task id
  auto* taskIdItem = taskTable_->item(row, col);
  if (!taskIdItem) {
    traceButton_->setEnabled(false);
    compareButton_->setEnabled(false);
  } else {
    // TODO: need backend API facility to clean this up
    auto task = TaskManager::GetTaskByID(selectedTaskId());
    traceButton_->setEnabled(bool(task));
    compareButton_->setEnabled(taskTable_->item(row, 4)->data(0).toBool());
  }
}

int TaskSelector::selectedTaskId() {
  auto row = taskTable_->currentRow();
  int col = 0;  // colume 0 is task id
  auto* taskIdItem = taskTable_->item(row, col);
  if (!taskIdItem) {
    log_infos << "No task selected";
    return 0;
  }
  return taskIdItem->data(0).toInt();
}

void TaskSelector::traceClicked() {
  auto id = selectedTaskId();
  log_debugs << "Tracing Task: " << id;
  auto task = TaskManager::GetTaskByID(id);
  TaskManager::LoadTask(task);
  accept();
}

void TaskSelector::compareClicked() {
  auto id = selectedTaskId();
  log_debugs << "Comparison Task: " << id;
  auto task = TaskManager::GetComparisonTaskByID(id);
  TaskManager::LoadTask(task);
  accept();
}

void TaskSelector::completedChanged() { getTasks(); }

uint8_t TaskSelector::datasetID() {
  if (!dataset()) {
    return 0;
  }
  return datasetCombo_->itemData(datasetCombo_->currentIndex()).toInt();
}

Dataset* TaskSelector::dataset() {
  if (!datasets_) {
    return nullptr;
  }
  int index = datasetCombo_->currentIndex();
  if (index >= datasets_->size()) {
    return nullptr;
  }
  return &(*datasets_)[index];
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
  auto tasks = TaskManager::GetTasks(
      dsid, cid,
      completedTasksCheckbox_->checkState() == Qt::Unchecked ? 1e6 : 0);
  taskTable_->setSortingEnabled(false);
  taskTable_->setRowCount(tasks->size());
  for (size_t i = 0; i < tasks->size(); ++i) {
    auto& t = (*tasks)[i];

    taskTable_->setItem(i, 0, makeTableItem(t.id));
    taskTable_->setItem(i, 1, makeTableItem(t.cell));
    taskTable_->setItem(i, 2, makeTableItem(t.parent_id));
    taskTable_->setItem(i, 3, makeTableItem(t.weight));
    taskTable_->setItem(
        i, 4, makeTableItem(t.inspected_weight == t.weight && t.weight > 1));
    taskTable_->setItem(i, 5, makeTableItem(QString::fromStdString(t.path)));
    taskTable_->setItem(
        i, 6, makeTableItem(QString::fromStdString(om::string::join(t.users))));
    taskTable_->setItem(i, 7, makeTableItem(QString::fromStdString(t.notes)));
  }

  taskTable_->setSortingEnabled(true);
  taskTable_->resizeColumnsToContents();
  // TODO
  taskTable_->sortByColumn(4);
  taskTable_->sortByColumn(2);
}

void TaskSelector::onManualEntry() {
  auto text = taskLineEdit_->text().trimmed();
  if (text.size()) {
    taskTable_->setSortingEnabled(false);
    taskTable_->setRowCount(0);
    taskTable_->setSortingEnabled(true);

    // TODO: clean up
    auto taskId = text.toInt();
    auto task = TaskManager::GetTaskByID(taskId);
    auto compTask = TaskManager::GetComparisonTaskByID(taskId);
    task = task ? task : compTask;
    if (task) {
      taskTable_->insertRow(0);
      taskTable_->setItem(0, 0, makeTableItem(taskId));
      taskTable_->setItem(0, 1, makeTableItem(task->CellId()));
      // taskTable_->setItem(i, 2, makeTableItem(t.weight));
      // taskTable_->setItem(i, 3, makeTableItem(t.inspected_weight ==
      // t.weight));
      taskTable_->setItem(
          0, 3, makeTableItem(bool(compTask)));  //...doesn't match real state
                                                 // taskTable_->setItem(i, 4,
      // makeTableItem(QString::fromStdString(t.path)));
      taskTable_->setCurrentCell(0, 0);
    }
  } else {
    getTasks();
  }
  updateEnabled();
}

void TaskSelector::accept() {
  OmLocalPreferences::setDataset(datasetID());
  QDialog::accept();
}
