#pragma once
#include "precomp.h"

#include "gui/sidebars/right/validationGroup/showValidWidget.hpp"
#include "gui/widgets/omButton.hpp"
#include "system/omConnect.hpp"
#include "viewGroup/omViewGroupState.h"

ShowValidWidget::ShowValidWidget(QWidget* widget, OmViewGroupState& vgs)
  : ViewGroupStateWidget(widget, vgs),
    validAsColor_(new QRadioButton("In Color")),
    validAsBlack_(new QRadioButton("As Black")),
    showValidButton_(new OmButton(this, "Show Validated",
          "Validated object mode", true)) {
  radioButtonGroup_ = new QButtonGroup();
  radioButtonGroup_->addButton(validAsColor_);
  radioButtonGroup_->addButton(validAsBlack_);

  QFormLayout* showValidForm = new QFormLayout(this);
  QWidget* layoutWidget = new QWidget(this);
  QGridLayout* gridLayout = new QGridLayout(layoutWidget);
  layoutWidget->setLayout(gridLayout);
  showValidForm->addWidget(layoutWidget);

  gridLayout->addWidget(showValidButton_, 0, 0, 1, 2);
  gridLayout->addWidget(validAsColor_, 1, 0, 1, 1);
  gridLayout->addWidget(validAsBlack_, 1, 1, 1, 1);
  validAsBlack_->setChecked(true);

  om::connect(validAsColor_, SIGNAL(toggled(bool)), this, SLOT(changeMapColors()));
  om::connect(validAsBlack_, SIGNAL(toggled(bool)), this,
              SLOT(changeMapColors()));
  om::connect(showValidButton_, SIGNAL(clicked(bool)), this, SLOT(changeMapColors()));
}

void ShowValidWidget::changeMapColors() {
  GetViewGroupState().SetShowValidMode(showValidButton_->isChecked(),
                                       validAsColor_->isChecked());
}
