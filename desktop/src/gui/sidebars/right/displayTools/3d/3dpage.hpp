#pragma once

#include "gui/sidebars/right/displayTools/3d/dust3DthresholdGroup.hpp"
#include "viewGroup/omViewGroupState.h"

#include <QtGui>

namespace om {
namespace displayTools {

class Page3d : public QWidget {
Q_OBJECT

private:
    OmViewGroupState *const vgs_;

public:
    Page3d(QWidget* parent, OmViewGroupState* vgs)
        : QWidget(parent)
        , vgs_(vgs)
    {
        QVBoxLayout* box = new QVBoxLayout(this);
        box->addWidget(thresholdBox());
        box->addStretch(1);
    }

private:

    QWidget* thresholdBox()
    {
        QGroupBox* widget = new QGroupBox("3D Dust Threshold", this);
        Dust3DThresholdGroup* thresholdBox = new Dust3DThresholdGroup(this, vgs_);

        QHBoxLayout* layout = new QHBoxLayout(widget);
        layout->addWidget(thresholdBox);

        return widget;
    }
};

} // namespace displayTools
} // namespace om
