#pragma once

#include "events/events.h"
#include "gui/sidebars/right/displayTools/2d/brightenSelected.hpp"
#include "gui/sidebars/right/displayTools/2d/brightnessSpinBox.hpp"
#include "gui/sidebars/right/displayTools/2d/contrastSpinBox.hpp"
#include "gui/sidebars/right/displayTools/2d/gammaSpinBox.hpp"
#include "gui/sidebars/right/displayTools/2d/alphaVegasButton.hpp"
#include "gui/sidebars/right/displayTools/displayTools.h"
#include "gui/widgets/omLabelHBox.hpp"
#include "system/omConnect.hpp"
#include "utility/dataWrappers.h"
#include "viewGroup/omViewGroupState.h"

#include <QtGui>

namespace om {
namespace displayTools {

class Page2d : public QWidget {
  Q_OBJECT;

 private:
  OmViewGroupState* const vgs_;

  QButtonGroup* validGroup_;
  QRadioButton* showValid_;
  QRadioButton* dontShowValid_;

 public:
  Page2d(QWidget* parent, OmViewGroupState* vgs) : QWidget(parent), vgs_(vgs) {
    QVBoxLayout* box = new QVBoxLayout(this);
    box->addWidget(filterShowNonSelectedSegmentsBox());
    box->addWidget(new BrightenSelected(this, vgs_));
    box->addWidget(imageFilterControls());
    box->addWidget(new AlphaVegasButton(this));
  }

 private
Q_SLOTS:
  void changeMapColorsSlot() {
    const bool val = showValid_->isChecked();
    vgs_->SetHowNonSelectedSegmentsAreColoredInFilter(val);
  }

 private:

  QWidget* filterShowNonSelectedSegmentsBox() {
    QGroupBox* widget = new QGroupBox("Show Non-selected Segments", this);

    validGroup_ = new QButtonGroup();
    showValid_ = new QRadioButton("In Color");
    validGroup_->addButton(showValid_);
    om::connect(showValid_, SIGNAL(toggled(bool)), this,
                SLOT(changeMapColorsSlot()));

    dontShowValid_ = new QRadioButton("As Black");
    dontShowValid_->setChecked(true);
    validGroup_->addButton(dontShowValid_);
    om::connect(dontShowValid_, SIGNAL(toggled(bool)), this,
                SLOT(changeMapColorsSlot()));

    QGridLayout* box = new QGridLayout(widget);
    box->addWidget(showValid_, 3, 0, 1, 1);
    box->addWidget(dontShowValid_, 3, 1, 1, 1);

    return widget;
  }

  QWidget* imageFilterControls() {
    QGroupBox* widget = new QGroupBox("Image Filters", this);
    QVBoxLayout* box = new QVBoxLayout(widget);

    box->addWidget(makeWidget(new BrightnessSpinBox(widget)));
    box->addWidget(makeWidget(new ContrastSpinBox(widget)));
    box->addWidget(makeWidget(new GammaSpinBox(widget)));

    box->setContentsMargins(10, 0, 5, 0);

    return widget;
  }

  template <typename W> QWidget* makeWidget(W* w) {
    QWidget* ret = new QWidget(this);
    QHBoxLayout* box = new QHBoxLayout(ret);

    box->addWidget(new QLabel(w->Label(), ret));
    box->addWidget(w);

    box->setContentsMargins(0, 0, 0, 0);

    return ret;
  }
};

}  // namespace displayTools
}  // namespace om
