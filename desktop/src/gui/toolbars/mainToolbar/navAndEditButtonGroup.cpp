#include "events/events.h"
#include "gui/toolbars/mainToolbar/navAndEditButtonGroup.h"
#include "gui/widgets/toolButton.hpp"
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
      new ToolButton(parent, "Validate", "Validate", om::tool::VALIDATE,
                     ":/toolbars/mainToolbar/icons/thumb-up-8x.png"));

  addButton(
      new ToolButton(parent, "Grow",
                     "Grow Tools", om::tool::GROW,
                     ":/toolbars/mainToolbar/icons/growing.png"));
}

int NavAndEditButtonGroup::addButton(ToolButton* button) {
  QButtonGroup::addButton(button);
  const int id = QButtonGroup::id(button);
  return id;
}
