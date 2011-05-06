#include "gui/sidebars/right/displayTools/2d/2dpage.hpp"
#include "gui/sidebars/right/displayTools/3d/3dpage.hpp"
#include "gui/sidebars/right/displayTools/sliceDepthSpinBoxX.hpp"
#include "gui/sidebars/right/displayTools/sliceDepthSpinBoxY.hpp"
#include "gui/sidebars/right/displayTools/sliceDepthSpinBoxZ.hpp"

DisplayTools::DisplayTools(DendToolBar* d, OmViewGroupState* vgs)
    : QWidget(d)
    , vgs_(vgs)
{
    QVBoxLayout* box = new QVBoxLayout(this);
    box->addWidget(view2dSliceDepthBox());

    QTabWidget* tabs = new QTabWidget(this);
    tabs->addTab(new om::displayTools::Page2d(this, GetViewGroupState()), "2D");
    tabs->addTab(new om::displayTools::Page3d(this, GetViewGroupState()), "3D");

    box->addWidget(tabs);
}

void DisplayTools::updateGui()
{
    OmEvents::Redraw2d();
    OmEvents::Redraw3d();
}

QWidget* DisplayTools::view2dSliceDepthBox()
{
    QGroupBox* widget = new QGroupBox("Slice Depths (x,y,z)", this);
    QHBoxLayout* layout = new QHBoxLayout(widget);

    layout->addWidget(new SliceDepthSpinBoxX(this, vgs_));
    layout->addWidget(new SliceDepthSpinBoxY(this, vgs_));
    layout->addWidget(new SliceDepthSpinBoxZ(this, vgs_));

    return widget;
}
