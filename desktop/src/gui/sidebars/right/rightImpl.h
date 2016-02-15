#pragma once
#include "precomp.h"

#include "common/common.h"
#include "gui/tools.hpp"

class GraphTools;
class MainWindow;
class OmViewGroupState;
class SegmentationDataWrapper;

namespace om {
namespace sidebars {

class rightImpl : public QWidget {
  Q_OBJECT;

 public:
  rightImpl(MainWindow* mw, OmViewGroupState& vgs);

  void updateGui();

  void SetJoiningSplittingOff(om::tool::mode tool);
  void SetShatteringOff();

  SegmentationDataWrapper GetSDW();

  static bool GetShowGroupsMode();

 private:
  template <class T>
  QWidget* wrapWithGroupBox(T* widget) {
    QGroupBox* gbox = new QGroupBox(widget->getName(), this);
    QVBoxLayout* vbox = new QVBoxLayout(gbox);
    vbox->addWidget(widget);
    gbox->setContentsMargins(0, 0, 0, 0);

    return gbox;
  }

  MainWindow* const mainWindow_;

  OmViewGroupState& vgs_;

  GraphTools* graphTools_;
};

}  // namespace sidebars
}  // namespace om
