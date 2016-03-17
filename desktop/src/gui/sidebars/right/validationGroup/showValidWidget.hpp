#pragma once
#include "precomp.h"

#include "gui/widgets/omButton.hpp"
#include "gui/widgets/viewGroupStateWidget.hpp"

class OmViewGroupState;

class ShowValidWidget : public ViewGroupStateWidget {
Q_OBJECT

 public:
  ShowValidWidget(QWidget* widget, OmViewGroupState& vgs);

  QString getName() { return "ShowValidWidget"; }

 private:
  QButtonGroup* radioButtonGroup_;
  QRadioButton* validAsColor_;
  QRadioButton* validAsBlack_;
  OmButton* showValidButton_;

 private
Q_SLOTS:
  void changeMapColors();

};
