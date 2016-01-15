#include "events/events.h"
#include "gui/toolbars/mainToolbar/navAndEditButtonGroup.h"
#include "gui/toolbars/mainToolbar/toolButton.h"
#include "gui/toolbars/mainToolbar/toolButton.h"
#include "system/omConnect.hpp"
#include "system/omStateManager.h"

NavAndEditButtonGroup::NavAndEditButtonGroup(QWidget* parent)
    : QButtonGroup(parent) {
  addButton(new ToolButton(
      parent, "Pan", "Move Image", om::tool::PAN,
      ":/toolbars/mainToolbar/icons/1277962397_cursor_hand.png"));

  addButton(new ToolButton(
      parent, "Select", "Select Object", om::tool::SELECT,
      ":/toolbars/mainToolbar/icons/1278008858_cursor_arrow.png"));

  addButton(
      new ToolButton(parent, "Brush", "Paint", om::tool::PAINT,
                     ":/toolbars/mainToolbar/icons/1277962300_paint.png"));

  addButton(new ToolButton(
      parent, "Eraser", "Paint Black Voxel", om::tool::ERASE,
      ":/toolbars/mainToolbar/icons/1277962354_package-purge.png"));

  addButton(
      new ToolButton(parent, "Fill", "Paint Can", om::tool::FILL,
                     ":/toolbars/mainToolbar/icons/1278015539_color_fill.png"));

  addButton(new ToolButton(
      parent, "Landmarks", "Landmarks", om::tool::LANDMARK,
      ":/toolbars/mainToolbar/icons/1308021634_keditbookmarks.png"));

  addButton(
      new ToolButton(parent, "Cut", "Cut", om::tool::CUT,
                     ":/toolbars/mainToolbar/icons/1308183310_Scissors.png"));

  addButton(
      new ToolButton(parent, "Annotate", "Annotate", om::tool::ANNOTATE,
                     ":/toolbars/mainToolbar/icons/kcmfontinst.png"));

  addButton(
      new ToolButton(parent, "Kalina",
                     "Do Crazy Stuff", om::tool::KALINA,
                     ":/toolbars/mainToolbar/icons/1277962552_highlight.png"));

  om::connect(this, SIGNAL(buttonClicked(int)), this,
              SLOT(buttonWasClicked(int)));

  om::connect(this, SIGNAL(signalSetTool(om::tool::mode)), this,
              SLOT(findAndSetTool(om::tool::mode)));
}

int NavAndEditButtonGroup::addButton(ToolButton* button) {
  QButtonGroup::addButton(button);
  const int id = QButtonGroup::id(button);
  allToolsByID_[id] = button;
  allToolsByMode_[button->getToolMode()] = button;
  return id;
}

void NavAndEditButtonGroup::buttonWasClicked(const int id) {
  ToolButton* button = allToolsByID_.at(id);
  makeToolActive(button);
}

void NavAndEditButtonGroup::makeToolActive(ToolButton* button) {
  OmStateManager::SetToolModeAndSendEvent(button->getToolMode());
}

void NavAndEditButtonGroup::EnableModifyingWidgets(const bool toBeEnabled) {
  allToolsByMode_[om::tool::PAINT]->setEnabled(toBeEnabled);
  allToolsByMode_[om::tool::ERASE]->setEnabled(toBeEnabled);
  allToolsByMode_[om::tool::FILL]->setEnabled(toBeEnabled);

  ToolButton* panButton = allToolsByMode_[om::tool::PAN];
  panButton->setChecked(true);
  makeToolActive(panButton);
}

void NavAndEditButtonGroup::SetTool(const om::tool::mode tool) {
  signalSetTool(tool);
}

void NavAndEditButtonGroup::findAndSetTool(const om::tool::mode tool) {
  ToolButton* button = nullptr;
  auto it = allToolsByMode_.find(tool);
  if (it != allToolsByMode_.end()) {
    button = it->second;
  } else {
    throw om::ArgException("tool not found");
  }

  button->setChecked(true);
  makeToolActive(button);
}
