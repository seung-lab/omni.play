#include "gui/sidebars/left/taskInfo.h"
#include "gui/sidebars/left/segListToggle.h"
#include "task/task.h"
#include "task/taskManager.h"
#include "system/omAppState.hpp"
#include "system/omConnect.hpp"
#include "utility/segmentationDataWrapper.hpp"

#include <vector>
#include <tuple>
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
  doneButton_ = new QPushButton(tr("Done"), this);
  om::connect(doneButton_, SIGNAL(clicked()), this, SLOT(doneButtonPressed()));
  layout->addRow(taskSelectorButton_, doneButton_);

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
    const auto& groups = task->SegGroups();

    // Order by size
    std::vector<std::tuple<size_t, std::string, const om::common::SegIDSet*>>
        orderedGroups;
    for (const auto& g : groups) {
      orderedGroups.push_back(std::make_tuple(getSize(g), g.first, &g.second));
    }
    std::sort(orderedGroups.begin(), orderedGroups.end());

    // Insert largest to smallest
    for (auto iter = orderedGroups.rbegin(); iter != orderedGroups.rend();
         ++iter) {
      buttonLayout->addRow(new om::gui::SegListToggleButton(
          buttons_, std::get<1>(*iter), *std::get<2>(*iter)));
    }
  }
}

size_t TaskInfoWidget::getSize(
    const std::pair<std::string, const om::common::SegIDSet>& group) const {
  size_t max = std::numeric_limits<size_t>::max();
  if (group.first.find("seed") != std::string::npos) {
    return max;
  } else if (group.first.find("agreed") != std::string::npos) {
    return max - 1;
  } else if (group.first.find("all") != std::string::npos) {
    return max - 2;
  } else if (group.first.find("dust") != std::string::npos) {
    return 0;
  }
  size_t size = 0;
  SegmentationDataWrapper sdw(1);
  if (sdw.IsValidWrapper()) {
    auto segments = sdw.Segments();
    for (const auto& id : group.second) {
      auto seg = segments->GetSegment(id);
      if (!seg) {
        continue;
      }
      size += seg->size();
    }
  }

  return size;
}

void TaskInfoWidget::taskSelectorButtonPressed() {
  OmAppState::OpenTaskSelector();
}

void TaskInfoWidget::doneButtonPressed() {
  if (TaskManager::FinishTask()) {
    OmAppState::OpenTaskSelector();
  }
}