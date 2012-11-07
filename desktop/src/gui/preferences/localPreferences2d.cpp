#include "common/omDebug.h"
#include "gui/preferences/checkboxHideCrosshairs.hpp"
#include "gui/preferences/localPreferences2d.h"
#include "gui/preferences/spinBoxCrosshairOpeningSize.hpp"
#include "gui/preferences/mipRateSpinBox.hpp"
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
                                    om::LEFT_SIDE,
                                    "Crosshair Opening Size"));

    layout->addWidget(new OmLabelHBox(widget,
                      				  new MipRateSpinBox(widget),
                      				  om::LEFT_SIDE,
                      				  "Mip Rate"));

    return widget;
}
