#pragma once
#include "precomp.h"

#include "gui/sidebars/right/displayTools/location/sliceDepthSpinBoxX.hpp"
#include "gui/sidebars/right/displayTools/location/sliceDepthSpinBoxY.hpp"
#include "gui/sidebars/right/displayTools/location/sliceDepthSpinBoxZ.hpp"
#include "gui/sidebars/right/displayTools/location/saveLocationButton.hpp"

class OmViewGroupState;

namespace om {
namespace displayTools {

class PageLocation : public QWidget {
  Q_OBJECT;

 private:
  OmViewGroupState* const vgs_;

 public:
  PageLocation(QWidget* parent, OmViewGroupState* vgs)
      : QWidget(parent), vgs_(vgs) {
    QVBoxLayout* box = new QVBoxLayout(this);

    box->addWidget(makeWidget(new SliceDepthSpinBoxX(this, vgs_)));
    box->addWidget(makeWidget(new SliceDepthSpinBoxY(this, vgs_)));
    box->addWidget(makeWidget(new SliceDepthSpinBoxZ(this, vgs_)));
    box->addWidget(new saveLocationButton(this, vgs_));
    box->addStretch(1);
  }

 private:
  QWidget* makeWidget(SliceDepthSpinBoxBase* w) {
    QWidget* ret = new QWidget(this);
    QHBoxLayout* box = new QHBoxLayout(ret);

    box->addWidget(new QLabel(w->Label(), ret));
    box->addWidget(w, 1);

    box->setContentsMargins(0, 0, 0, 0);

    return ret;
  }
};

}  // namespace displayTools
}  // namespace om
