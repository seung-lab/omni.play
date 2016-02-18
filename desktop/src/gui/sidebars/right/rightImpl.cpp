#include "viewGroup/omViewGroupState.h"
#include "common/logging.h"
#include "events/events.h"
#include "gui/mainWindow/mainWindow.h"
#include "gui/sidebars/right/rightImpl.h"
#include "gui/sidebars/right/displayTools/displayTools.h"
#include "gui/sidebars/right/graphTools/graphTools.h"
#include "gui/sidebars/right/validationGroup/validationGroup.h"
#include "gui/sidebars/right/annotationGroup/annotationGroup.h"
#include "gui/widgets/omWidget.hpp"
#include "system/omAppState.hpp"
#include "utility/dataWrappers.h"

om::sidebars::rightImpl::rightImpl(MainWindow* mainWindow,
                                   OmViewGroupState& vgs)
    : QWidget(mainWindow), mainWindow_(mainWindow), vgs_(vgs) {
  OmAppState::SetRightToolBar(this);

  graphTools_ = new GraphTools(this, vgs_);
  ValidationGroup* validationGroup = new ValidationGroup(this, vgs_);
  DisplayTools* displayTools = new DisplayTools(this, vgs_);
  AnnotationGroup* annotationGroup = new AnnotationGroup(this, vgs_);

  QVBoxLayout* vbox = new QVBoxLayout(this);
  vbox->addWidget(wrapWithGroupBox(graphTools_));
  vbox->addWidget(wrapWithGroupBox(validationGroup));
  vbox->addWidget(wrapWithGroupBox(displayTools));
  vbox->addWidget(wrapWithGroupBox(annotationGroup), 1);
  setMaximumSize(250, 2000);
}

SegmentationDataWrapper om::sidebars::rightImpl::GetSDW() {
  return vgs_.Segmentation();
}

void om::sidebars::rightImpl::updateGui() { om::event::Redraw2d(); om::event::Redraw3d(); }
