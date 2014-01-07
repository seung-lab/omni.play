#include "gui/sidebars/left/taskInfo.h"
#include "gui/sidebars/left/segListToggle.h"
#include "task/task.h"
#include "task/taskManager.h"
#include "system/omAppState.hpp"
#include "system/omConnect.hpp"

#include <QtGui>

using namespace om::task;

TaskInfoWidget::TaskInfoWidget(QWidget* parent)
    : QWidget(parent), buttons_(nullptr) {

  auto layout = new QFormLayout(this);

  QLabel* id = new QLabel(tr("Cube ID"), this);
  idLabel_ = new QLabel(this);
  layout->addRow(id, idLabel_);

  QLabel* cellID = new QLabel(tr("Cell ID"), this);
  cellIdLabel_ = new QLabel(this);
  layout->addRow(cellID, cellIdLabel_);

  taskSelectorButton_ = new QPushButton(tr("Skip"), this);
  om::connect(taskSelectorButton_, SIGNAL(clicked()), this,
              SLOT(taskSelectorButtonPressed()));
  layout->addRow(taskSelectorButton_);

  doneButton_ = new QPushButton(tr("Done"), this);
  om::connect(doneButton_, SIGNAL(clicked()), this, SLOT(doneButtonPressed()));
  layout->addRow(doneButton_);
  updateInfo();
}

TaskInfoWidget::~TaskInfoWidget() {}

void TaskInfoWidget::TaskStartedEvent() { updateInfo(); }

void TaskInfoWidget::updateInfo() {
  auto task = om::task::TaskManager::currentTask().get();

  idLabel_->setText(task ? QString::number(task->Id()) : tr(""));
  cellIdLabel_->setText(task ? QString::number(task->CellId()) : tr(""));

  auto l = dynamic_cast<QFormLayout*>(layout());

  if (buttons_) {
    l->removeWidget(buttons_);
    delete buttons_;
  }
  buttons_ = new QFrame(this);
  l->insertRow(2, buttons_);
  auto buttonLayout = new QFormLayout(buttons_);
  if (task) {
    for (auto& g : task->SegGroups()) {
      buttonLayout->addRow(
          new om::gui::SegListToggleButton(buttons_, g.first, g.second));
    }
  }
}

void TaskInfoWidget::taskSelectorButtonPressed() {
  OmAppState::OpenTaskSelector();
}

void TaskInfoWidget::doneButtonPressed() {
  if (TaskManager::FinishTask()) {
    OmAppState::OpenTaskSelector();
  }
}