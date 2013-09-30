#include "common/enums.hpp"
#include "common/logging.h"
#include "gui/preferences/checkboxHideCrosshairs.hpp"
#include "gui/preferences/localPreferences2d.h"
#include "gui/preferences/spinBoxCrosshairOpeningSize.hpp"
#include "gui/preferences/mipRateSpinBox.hpp"
#include "gui/preferences/scrollRateSpinBox.hpp"
#include "gui/widgets/omLabelHBox.hpp"

#include <QtGui>

LocalPreferences2d::LocalPreferences2d(QWidget * parent)
    : QWidget(parent)
{
    QVBoxLayout* overallContainer = new QVBoxLayout(this);

    overallContainer->addWidget( makeGeneralPropBox());
    overallContainer->insertStretch(2, 1);
}

QGroupBox* LocalPreferences2d::makeGeneralPropBox()
{
    QGroupBox* widget = new QGroupBox("General");
    QVBoxLayout* layout = new QVBoxLayout(widget);

    layout->addWidget(new ShowCrosshairsCheckbox(this));

    layout->addWidget(new OmLabelHBox(widget,
                                      new CrosshairOpeningSizeSpinBox(widget),
                                      om::Side::LEFT_SIDE,
                                      "Crosshair Opening Size"));

    layout->addWidget(new OmLabelHBox(widget,
                                      new MipRateSpinBox(widget),
                                      om::Side::LEFT_SIDE,
                                      "Mip Rate"));

    layout->addWidget(new OmLabelHBox(widget,
                                      new ScrollRateSpinBox(widget),
                                      om::Side::LEFT_SIDE,
                                      "Scroll Rate"));

    return widget;
}
