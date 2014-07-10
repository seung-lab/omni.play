#include "gui/sidebars/right/displayTools/2d/2dpage.hpp"
#include "gui/sidebars/right/displayTools/3d/3dpage.hpp"
#include "gui/sidebars/right/displayTools/location/pageLocation.hpp"
#include "gui/sidebars/right/rightImpl.h"

DisplayTools::DisplayTools(om::sidebars::rightImpl* d, OmViewGroupState& vgs)
    : QWidget(d), vgs_(vgs) {
  QVBoxLayout* box = new QVBoxLayout(this);

  QTabWidget* tabs = new QTabWidget(this);
  tabs->addTab(new om::displayTools::PageLocation(this, GetViewGroupState()),
               "Location");

  box->addWidget(tabs);
}

void DisplayTools::updateGui() {
  om::event::Redraw2d();
  om::event::Redraw3d();
}
