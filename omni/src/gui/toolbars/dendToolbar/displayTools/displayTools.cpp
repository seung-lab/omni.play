#include "system/omConnect.hpp"
#include "gui/toolbars/dendToolbar/alphaVegasButton.hpp"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
#include "gui/toolbars/dendToolbar/displayTools/brightnessSpinBox.hpp"
#include "gui/toolbars/dendToolbar/displayTools/contrastSpinBox.hpp"
#include "gui/toolbars/dendToolbar/displayTools/gammaSpinBox.hpp"
#include "gui/toolbars/dendToolbar/displayTools/displayTools.h"
#include "gui/toolbars/dendToolbar/displayTools/dust3DthresholdGroup.hpp"
#include "gui/toolbars/dendToolbar/displayTools/sliceDepthSpinBoxX.hpp"
#include "gui/toolbars/dendToolbar/displayTools/sliceDepthSpinBoxY.hpp"
#include "gui/toolbars/dendToolbar/displayTools/sliceDepthSpinBoxZ.hpp"
#include "gui/widgets/omLabelHBox.hpp"
#include "events/omEvents.h"
#include "utility/dataWrappers.h"
#include "viewGroup/omViewGroupState.h"

DisplayTools::DisplayTools(DendToolBar * d)
    : OmWidget(d)
    , mParent(d)
{
    QVBoxLayout* box = new QVBoxLayout(this);
    box->addWidget(thresholdBox());
    box->addWidget(filterShowNonSelectedSegmentsBox());
    box->addWidget(view2dSliceDepthBox());
    box->addWidget(imageFilterControls());
    box->addWidget(new AlphaVegasButton(this));
}

QWidget* DisplayTools::imageFilterControls()
{
    QGroupBox* widget = new QGroupBox("Image Filters", this);
    QVBoxLayout* layout = new QVBoxLayout(widget);

    layout->addWidget(new OmLabelHBox(widget,
                                      new BrightnessSpinBox(this),
                                      om::LEFT_SIDE,
                                      "Brightness"));

    layout->addWidget(new OmLabelHBox(widget,
                                      new ContrastSpinBox(this),
                                      om::LEFT_SIDE,
                                      "Contrast"));
    layout->addWidget(new OmLabelHBox(widget,
                                      new GammaSpinBox(this),
                                      om::LEFT_SIDE,
                                      "Gamma"));
    return widget;
}

QWidget* DisplayTools::view2dSliceDepthBox()
{
    QGroupBox* widget = new QGroupBox("Slice Depths (x,y,z)", this);
    QHBoxLayout* layout = new QHBoxLayout(widget);

    layout->addWidget(new SliceDepthSpinBoxX(this));
    layout->addWidget(new SliceDepthSpinBoxY(this));
    layout->addWidget(new SliceDepthSpinBoxZ(this));

    return widget;
}

void DisplayTools::changeMapColorsSlot()
{
    const bool val = showValid->isChecked();

    getViewGroupState()->SetHowNonSelectedSegmentsAreColoredInFilter(val);
}

QWidget* DisplayTools::filterShowNonSelectedSegmentsBox()
{
    QGroupBox* widget = new QGroupBox("Show Non-selected Segments", this);

    validGroup = new QButtonGroup();
    showValid = new QRadioButton("In Color");
    validGroup->addButton(showValid);
    om::connect(showValid, SIGNAL(toggled(bool)),
            this, SLOT(changeMapColorsSlot()));

    dontShowValid = new QRadioButton("As Black");
    dontShowValid->setChecked(true);
    validGroup->addButton(dontShowValid);
    om::connect(dontShowValid, SIGNAL(toggled(bool)),
            this, SLOT(changeMapColorsSlot()));

    QGridLayout * box = new QGridLayout(widget);
    box->addWidget(showValid,3,0,1,1);
    box->addWidget(dontShowValid,3,1,1,1);

    return widget;
}

QWidget* DisplayTools::thresholdBox()
{
    QGroupBox* widget = new QGroupBox("3D Dust Threshold", this);
    Dust3DThresholdGroup* thresholdBox = new Dust3DThresholdGroup(this);

    QHBoxLayout* layout = new QHBoxLayout(widget);
    layout->addWidget(thresholdBox);

    return widget;
}

OmViewGroupState * DisplayTools::getViewGroupState()
{
    return mParent->getViewGroupState();
}

void DisplayTools::updateGui()
{
    OmEvents::Redraw2d();
}

SegmentationDataWrapper DisplayTools::GetSDW()
{
    return mParent->GetSDW();
}
