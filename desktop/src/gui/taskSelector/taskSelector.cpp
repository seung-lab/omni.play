#include "gui/taskSelector/taskSelector.h"
#include "system/omLocalPreferences.hpp"
#include "events/events.h"
#include "gui/exec.hpp"
#include "task/comparisonTask.h"

using namespace om::task;

enum class Columns {
  Id = 0,
  Cell,
  Parent,
  Weight,
  Comparison,
  Path,
  Users,
  Notes
};

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
              SLOT(updateList()));

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
  taskTable_->setEditTriggers(QAbstractItemView::DoubleClicked);

  layout->addWidget(taskTable_, 1, 0, 1, 6);
  om::connect(taskTable_, SIGNAL(itemSelectionChanged()), this,
              SLOT(updateEnabled()));
  om::connect(taskTable_, SIGNAL(itemChanged(QTableWidgetItem*)), this,
              SLOT(itemEdited(QTableWidgetItem*)));

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

  TaskManager::Refresh();
  datasetsRequest_ = TaskManager::GetDatasets();
  *datasetsRequest_ >>= [this](
      std::shared_ptr<std::vector<om::task::Dataset>> datasets) {
    auto oldDatasetSelection = datasetID();
    datasetCombo_->clear();
    if ((bool)datasets) {
      if (!oldDatasetSelection) {
        oldDatasetSelection = OmLocalPreferences::getDataset();
      }
      int i = 0;
      for (auto& ds : *datasets) {
        datasetCombo_->addItem(
            QString::fromStdString(std::to_string(ds.id()) + ". " + ds.name()),
            ds.id());
        if (ds.id() == oldDatasetSelection) {
          datasetCombo_->setCurrentIndex(i);
        }
        i++;
      }
      getTasks();
    }
  };
}

void TaskSelector::updateCells() {
  cellCombo_->clear();
  cellCombo_->addItem("", 0);
  if (dataset()) {
    cellsRequest_ = om::task::TaskManager::GetCells(dataset()->id());
    *cellsRequest_ >>= [this](std::shared_ptr<std::vector<Cell>> cells) {
      if (!cells) {
        return;
      }
      for (int i = 0; i < cells->size(); ++i) {
        const Cell& c = (*cells)[i];
        cellCombo_->addItem(
            QString::fromStdString(std::to_string(c.CellID) + " - " + c.Name),
            i + 1);
        if (c.CellID == oldCellSelection_) {
          cellCombo_->setCurrentIndex(i + 1);
        }
      }
    };
  }
}

void TaskSelector::updateList() {
  getTasks();
  updateEnabled();
}

void TaskSelector::updateEnabled() {
  auto row = taskTable_->currentRow();
  auto* taskIdItem = taskTable_->item(row, (int)Columns::Id);
  if (!taskIdItem) {
    traceButton_->setEnabled(false);
    compareButton_->setEnabled(false);
  } else {
    traceButton_->setEnabled(true);
    compareButton_->setEnabled(
        taskTable_->item(row, (int)Columns::Comparison)->data(0).toBool());
  }
}

int TaskSelector::selectedTaskId() {
  auto row = taskTable_->currentRow();
  auto* taskIdItem = taskTable_->item(row, (int)Columns::Id);
  if (!taskIdItem) {
    log_infos << "No task selected";
    return 0;
  }
  return taskIdItem->data(0).toInt();
}

void TaskSelector::traceClicked() {
  auto id = selectedTaskId();
  log_debugs << "Tracing Task: " << id;
  taskRequest_ = TaskManager::GetTaskByID(id);
  *taskRequest_ >>= [this](std::shared_ptr<TracingTask> t) {
    if (t) {
      TaskManager::LoadTask(std::static_pointer_cast<Task>(t));
      accept();
    }
  };
}

void TaskSelector::compareClicked() {
  auto id = selectedTaskId();
  log_debugs << "Comparison Task: " << id;
  compTaskRequest_ = TaskManager::GetComparisonTaskByID(id);
  *compTaskRequest_ >>= [this](std::shared_ptr<ComparisonTask> t) {
    if (t) {
      TaskManager::LoadTask(std::static_pointer_cast<Task>(t));
      accept();
    }
  };
}

void TaskSelector::completedChanged() { getTasks(); }

uint8_t TaskSelector::datasetID() {
  if (!dataset()) {
    return 0;
  }
  return datasetCombo_->itemData(datasetCombo_->currentIndex()).toInt();
}

Dataset* TaskSelector::dataset() {
  if (!datasetsRequest_) {
    return nullptr;
  }
  auto datasets = datasetsRequest_->result();
  if (!datasets) {
    return nullptr;
  }
  int index = datasetCombo_->currentIndex();
  if (index >= datasets->size()) {
    return nullptr;
  }
  return &(*datasets)[index];
}

uint32_t TaskSelector::cellID() {
  if (!cellsRequest_) {
    return 0;
  }
  auto cells = cellsRequest_->result();
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

void TaskSelector::itemEdited(QTableWidgetItem* item) {
  if (item->column() == (int)Columns::Notes) {
    auto row = item->row();
    auto id = taskTable_->item(row, (int)Columns::Id)->data(0).toInt();
    if (!TaskManager::UpdateNotes(id, item->data(0).toString().toStdString())) {
      item->setData(0, "");
    }
  }
}

uint32_t TaskSelector::taskID() { return taskLineEdit_->text().toInt(); }

template <typename T>
QTableWidgetItem* makeTableItem(const T val,
                                Qt::ItemFlags flags =
                                    Qt::ItemIsEnabled | Qt::ItemIsSelectable) {
  QTableWidgetItem* item = new QTableWidgetItem();
  item->setData(0, val);
  item->setFlags(flags);
  return item;
}

void TaskSelector::updateTasks(const std::vector<TaskInfo>& tasks) {
  taskTable_->setSortingEnabled(false);
  taskTable_->setRowCount(tasks.size());
  taskTable_->blockSignals(true);
  for (size_t i = 0; i < tasks.size(); ++i) {
    auto& t = tasks[i];

    taskTable_->setItem(i, (int)Columns::Id, makeTableItem(t.id));
    taskTable_->setItem(i, (int)Columns::Cell, makeTableItem(t.cell));
    taskTable_->setItem(i, (int)Columns::Parent, makeTableItem(t.parent));
    taskTable_->setItem(i, (int)Columns::Weight, makeTableItem(t.weight));
    taskTable_->setItem(
        i, (int)Columns::Comparison,
        makeTableItem(t.inspected_weight == t.weight && t.weight > 1));
    taskTable_->setItem(i, (int)Columns::Path,
                        makeTableItem(QString::fromStdString(t.path)));
    taskTable_->setItem(i, (int)Columns::Users,
                        makeTableItem(QString::fromStdString(t.users)));
    taskTable_->setItem(i, (int)Columns::Notes,
                        makeTableItem(QString::fromStdString(t.notes),
                                      Qt::ItemIsEnabled | Qt::ItemIsSelectable |
                                          Qt::ItemIsEditable));
  }

  taskTable_->setSortingEnabled(true);
  taskTable_->resizeColumnsToContents();
  // TODO
  taskTable_->sortByColumn((int)Columns::Weight);
  taskTable_->sortByColumn((int)Columns::Cell);
  taskTable_->blockSignals(false);
}

void TaskSelector::getTasks() {
  taskTable_->setRowCount(0);
  auto text = taskLineEdit_->text().trimmed();
  auto taskId = text.toInt();
  if (taskId) {
    tasksRequest_.reset();
    taskRequest_ = TaskManager::GetTaskByID(taskId);
    compTaskRequest_ = TaskManager::GetComparisonTaskByID(taskId);

    *taskRequest_ >>= [this](std::shared_ptr<om::task::TracingTask> task) {
      if (task) {
        std::vector<TaskInfo> tasks = {
            TaskInfo{(uint32_t)task->Id(), task->Weight(),   task->WeightSum(),
                     task->Path(),         task->CellId(),   0,
                     0,                    0,                task->Status(),
                     task->Users(),        task->ParentID(), task->Notes()}};
        updateTasks(tasks);
      }
    };
  } else {
    taskRequest_.reset();
    compTaskRequest_.reset();
    tasksRequest_ = TaskManager::GetTasks(
        datasetID(), cellID(),
        completedTasksCheckbox_->checkState() == Qt::Unchecked ? 1e6 : 0);

    *tasksRequest_ >>= [this](std::shared_ptr<std::vector<TaskInfo>> tasks) {
      if (tasks) {
        updateTasks(*tasks);
      }
    };
  }
}

void TaskSelector::accept() {
  OmLocalPreferences::setDataset(datasetID());
  QDialog::accept();
}
