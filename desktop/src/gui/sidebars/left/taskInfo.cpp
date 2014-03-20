#include "gui/sidebars/left/taskInfo.h"
#include "gui/sidebars/left/segListToggle.h"
#include "task/task.h"
#include "task/taskManager.h"
#include "system/omAppState.hpp"
#include "system/omConnect.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "actions/omActions.h"
#include "segment/omSegment.h"

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

void TaskInfoWidget::TaskChangeEvent() {
  if (TaskManager::currentTask()) {
    doneButton_->setEnabled(true);
    taskSelectorButton_->setText(tr("Skip"));
  } else {
    doneButton_->setEnabled(false);
    taskSelectorButton_->setText(tr("New task"));
  }
}

void TaskInfoWidget::updateInfo() {
  auto task = om::task::TaskManager::currentTask().get();

  idLabel_->setText(task ? QString::number(task->Id()) : tr(""));
  cellIdLabel_->setText(task ? QString::number(task->CellId()) : tr(""));

  buttons_ = new QFrame(this);
  auto buttonLayout = new QGridLayout(buttons_);
  if (task) {
    const auto& groups = task->SegGroups();

    // Order by type and size
    std::map<om::task::SegGroup::GroupType, int> typeorder{
        {SegGroup::GroupType::SEED, 1000},
        {SegGroup::GroupType::ALL, 0},
        {SegGroup::GroupType::AGREED, 100},
        {SegGroup::GroupType::USER, 10},
        {SegGroup::GroupType::DUST, 1},
        {SegGroup::GroupType::PARTIAL, 50}, };
    std::vector<std::tuple<int, std::string, size_t, const om::task::SegGroup*>>
        orderedGroups;
    for (const auto& g : groups) {
      if (g.segments.size() > 0) {
        orderedGroups.push_back(
            std::make_tuple(typeorder[g.type], g.name, getSize(g), &g));
      }
    }
    std::sort(orderedGroups.begin(), orderedGroups.end());

    // Insert largest to smallest
    auto makeButton = [&](
        std::tuple<int, std::string, size_t, const om::task::SegGroup*>& tup) {
      auto text = std::get<1>(tup) + " : " + std::to_string(std::get<2>(tup));
      return new om::gui::SegListToggleButton(buttons_, text,
                                              std::get<3>(tup)->segments);
    };
    auto iter = orderedGroups.rbegin();
    if (iter != orderedGroups.rend()) {
      buttonLayout->addWidget(makeButton(*iter), 0, 0);
      ++iter;
    }
    if (iter != orderedGroups.rend()) {
      buttonLayout->addWidget(makeButton(*iter), 0, 1);
      ++iter;
    }
    int nRows = (orderedGroups.size() + 1) / 2;
    int count = 0;
    for (; iter != orderedGroups.rend(); ++iter) {
      buttonLayout->addWidget(makeButton(*iter), count % (nRows - 1) + 1,
                              count / (nRows - 1));
      count++;
    }
  }
  scrollable_->setWidget(buttons_);
}

size_t TaskInfoWidget::getSize(const om::task::SegGroup& group) const {
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
  QMessageBox box(
      QMessageBox::Question, "Save project?",
      "Do you want to save current project?",
      QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
  int result = box.exec();
  if (result == QMessageBox::Cancel) {
    return;
  }
  if (result == QMessageBox::Save) {
    OmActions::Save();
  }
  OmAppState::OpenTaskSelector();
}

void TaskInfoWidget::doneButtonPressed() {
  QMessageBox box(
      QMessageBox::Question, "Submit current task?",
      "Would you like to submit your accomplishments on the current task?",
      QMessageBox::Yes | QMessageBox::Cancel);
  int result = box.exec();
  if (result == QMessageBox::Cancel) {
    return;
  }
  TaskManager::SubmitTask();
  OmActions::Save();
  OmAppState::OpenTaskSelector();
}
