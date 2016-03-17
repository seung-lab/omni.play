#include "common/logging.h"
#include "gui/sidebars/right/rightImpl.h"
#include "gui/sidebars/right/validationGroup/validButton.hpp"
#include "gui/sidebars/right/validationGroup/uncertainButton.hpp"
#include "gui/sidebars/right/validationGroup/validationGroup.h"
#include "gui/sidebars/right/validationGroup/showValidWidget.hpp"
#include "system/omConnect.hpp"
#include "utility/dataWrappers.h"
#include "utility/segmentationDataWrapper.hpp"
#include "viewGroup/omViewGroupState.h"

ValidationGroup::ValidationGroup(om::sidebars::rightImpl* d,
                                 OmViewGroupState& vgs)
    : OmWidget(d),
      vgs_(vgs),
      setSelectionValid(new ValidButton(this, vgs,
            om::common::SetValid::SET_VALID)),
      setSelectionNotValid(new ValidButton(this, vgs,
            om::common::SetValid::SET_NOT_VALID)),
      setSelectionUncertain(new UncertainButton(this, vgs, true)),
      setSelectionNotUncertain(new UncertainButton(this, vgs, false)),
      showValidWidget(new ShowValidWidget(this, vgs)) {
  mGroupName = new QLineEdit(this);
  mGroupName->setText("Glia");

  QFormLayout* form = new QFormLayout(this);
  QWidget* w = new QWidget(this);
  form->addWidget(w);

  QGridLayout* box = new QGridLayout(w);
  box->addWidget(addSelectedSegmentButtons(), 0, 0, 2, 2);
  box->addWidget(addSelectedSegmentButtons(), 0, 0, 2, 2);
  box->addWidget(showValidWidget, 1, 0, 1, 2);
  box->addWidget(mGroupName, 2, 1, 1, 1);
}

QWidget* ValidationGroup::addSelectedSegmentButtons() {
  QGroupBox* box = new QGroupBox("Set Selected Segments...", this);
  QGridLayout* layout = new QGridLayout(box);

  layout->addWidget(setSelectionValid, 0, 0, 1, 1);
  layout->addWidget(setSelectionNotValid, 1, 0, 1, 1);
  layout->addWidget(setSelectionUncertain, 0, 1, 1, 1);
  layout->addWidget(setSelectionNotUncertain, 1, 1, 1, 1);

  return box;
}

QString ValidationGroup::getGroupNameFromGUI() { return mGroupName->text(); }

OmViewGroupState& ValidationGroup::GetViewGroupState() const { return vgs_; }

SegmentationDataWrapper ValidationGroup::GetSDW() const {
  return vgs_.Segmentation();
}
