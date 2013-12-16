#include "gui/sidebars/left/taskInfo.h"
#include "task/task.h"
#include "task/taskManager.h"
#include "system/omAppState.hpp"
#include "system/omConnect.hpp"

#include <QtGui>

using namespace om::task;

TaskInfoWidget::TaskInfoWidget(QWidget* parent) : QWidget(parent) {
  auto layout = new QFormLayout(this);

  QLabel* id = new QLabel(tr("Cube ID"), this);
  idLabel_ = new QLabel(this);
  layout->addRow(id, idLabel_);

  QLabel* cellID = new QLabel(tr("Cell ID"), this);
  cellIdLabel_ = new QLabel(this);
  layout->addRow(cellID, cellIdLabel_);

  taskSelectorButton_ = new QPushButton(tr("Select Task"), this);
  om::connect(taskSelectorButton_, SIGNAL(clicked()), this,
              SLOT(taskSelectorButtonPressed()));
  layout->addRow(taskSelectorButton_);

  doneButton_ = new QPushButton(tr("Done"), this);
  om::connect(doneButton_, SIGNAL(clicked()), this, SLOT(doneButtonPressed()));
  layout->addRow(doneButton_);
  updateInfo();
}

TaskInfoWidget::~TaskInfoWidget() {}

void TaskInfoWidget::TaskChangeEvent() { updateInfo(); }

void TaskInfoWidget::updateInfo() {
  auto task = om::task::TaskManager::currentTask().get();

  idLabel_->setText(task ? QString::number(task->Id()) : tr(""));
  cellIdLabel_->setText(task ? QString::number(task->CellId()) : tr(""));
}
void TaskInfoWidget::taskSelectorButtonPressed() {
  OmAppState::OpenTaskSelector();
}

void TaskInfoWidget::doneButtonPressed() {
  if (TaskManager::FinishTask()) {
    OmAppState::OpenTaskSelector();
  }
}