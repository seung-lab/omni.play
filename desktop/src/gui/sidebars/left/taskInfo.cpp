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

  scrollable_ = new QScrollArea(this);
  layout->addRow(scrollable_);

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

  buttons_ = new QFrame(this);
  auto buttonLayout = new QVBoxLayout(buttons_);
  if (task) {
    const auto& groups = task->SegGroups();

    // Order by size
    std::vector<std::tuple<size_t, std::string, const om::task::SegGroup*>>
        orderedGroups;
    for (const auto& g : groups) {
      if (g.segments.size() > 0) {
        orderedGroups.push_back(std::make_tuple(getSize(g), g.name, &g));
      }
    }
    std::sort(orderedGroups.begin(), orderedGroups.end());

    // Insert largest to smallest
    for (auto iter = orderedGroups.rbegin(); iter != orderedGroups.rend();
         ++iter) {
      buttonLayout->addWidget(new om::gui::SegListToggleButton(
          buttons_, std::get<1>(*iter), std::get<2>(*iter)->segments));
    }
  }
  scrollable_->setWidget(buttons_);
}

size_t TaskInfoWidget::getSize(const om::task::SegGroup& group) const {
  size_t max = std::numeric_limits<size_t>::max();
  if (group.type == om::task::SegGroup::GroupType::SEED) {
    return max;
  } else if (group.type == om::task::SegGroup::GroupType::AGREED) {
    return max - 1;
  } else if (group.type == om::task::SegGroup::GroupType::ALL) {
    return max - 2;
  } else if (group.type == om::task::SegGroup::GroupType::DUST) {
    return 0;
  }
  size_t size = 0;
  SegmentationDataWrapper sdw(1);
  if (sdw.IsValidWrapper()) {
    auto segments = sdw.Segments();
    for (const auto& id : group.segments) {
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