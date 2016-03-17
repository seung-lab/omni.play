#include "events/events.h"
#include "gui/sidebars/right/rightImpl.h"
#include "gui/sidebars/right/graphTools/mstThresholdSpinBox.hpp"
#include "gui/sidebars/right/graphTools/automaticSpreadingThresholdSpinBox.hpp"
#include "gui/sidebars/right/graphTools/graphTools.h"
#include "gui/widgets/toolButton.hpp"
#include "gui/sidebars/right/graphTools/showBreakWidget.hpp"
#include "gui/sidebars/right/graphTools/joinAllButton.h"
#include "utility/dataWrappers.h"
#include "viewGroup/omViewGroupState.h"
#include "system/omStateManager.h"

GraphTools::GraphTools(om::sidebars::rightImpl* d, OmViewGroupState& vgs)
    : OmWidget(d),
      mParent(d),
      vgs_(vgs),
      joinButton(new ToolButton(
            this, "Join", "Join Objects", om::tool::mode::JOIN)),
      splitButton(new ToolButton(
            this, "Split", "Split Objects", om::tool::mode::SPLIT)),
      shatterButton(new ToolButton(
            this, "Shatter", "Shatter Objects", om::tool::mode::SHATTER)),
      joinAllButton(new JoinAllButton(this, vgs)),
      showBreakWidget(new ShowBreakWidget(this, vgs)) {
  QFormLayout* box = new QFormLayout(this);
  box->addWidget(thresholdBox());

  QWidget* wbox = new QWidget(this);
  QGridLayout* box2 = new QGridLayout(wbox);
  wbox->setLayout(box2);
  box->addWidget(wbox);

  box2->addWidget(joinButton, 0, 0, 1, 1);
  box2->addWidget(joinAllButton, 0, 1, 1, 1);
  box2->addWidget(splitButton, 1, 0, 1, 1);
  box2->addWidget(shatterButton, 1, 1, 1, 1);
  box2->addWidget(showBreakWidget, 2, 0, 1, 2);

}

QWidget* GraphTools::thresholdBox() {
  QGroupBox* widget = new QGroupBox("Minimum Affinity to Join", this);

  QLabel *mstThresholdLabel = new QLabel(tr("Global Threshold"));
  threshold_ = new MSTThresholdSpinBox(this);
  QLabel *automaticSpreadThresholdLabel = new QLabel(tr("Auto Spread"));
  asthreshold_ = new AutomaticSpreadingThresholdSpinBox(this);

  QVBoxLayout* layout = new QVBoxLayout(widget);
  layout->addWidget(mstThresholdLabel);
  layout->addWidget(threshold_);
  layout->addWidget(automaticSpreadThresholdLabel);
  layout->addWidget(asthreshold_);

  return widget;
}

SegmentationDataWrapper GraphTools::GetSDW() { return mParent->GetSDW(); }

QPushButton* GraphTools::getButton(om::tool::mode tool) {
  switch (tool) {
    case om::tool::mode::JOIN:
      return joinButton;
    case om::tool::mode::SPLIT:
      return splitButton;
    case om::tool::mode::SHATTER:
      return shatterButton;
    default:
      return nullptr;
  }
}
