#pragma once
#include "precomp.h"

#include "gui/widgets/omWidget.hpp"

class ValidButton;
class UncertainButton;
class SegmentationDataWrapper;
class OmViewGroupState;
class ShowValidWidget;

namespace om {
namespace sidebars {
class rightImpl;
}
}

class ValidationGroup : public OmWidget {
  Q_OBJECT;

 public:
  ValidationGroup(om::sidebars::rightImpl*, OmViewGroupState& vgs);
  bool isShowValidChecked();

  QString getName() { return "Validation"; }

  OmViewGroupState& GetViewGroupState() const;

  SegmentationDataWrapper GetSDW() const;

 private:
  OmViewGroupState& vgs_;

  ShowValidWidget* showValidWidget;

  ValidButton* setSelectionValid;
  ValidButton* setSelectionNotValid;

  UncertainButton* setSelectionUncertain;
  UncertainButton* setSelectionNotUncertain;

  QWidget* addSelectedSegmentButtons();
};
