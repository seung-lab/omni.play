#include "gui/mainWindow/mainWindow.h"
#include "gui/toolbars/mainToolbar/filterToBlackCheckbox.hpp"
#include "gui/toolbars/mainToolbar/filterWidget.hpp"
#include "gui/toolbars/mainToolbar/mainToolbar.h"
#include "gui/toolbars/mainToolbar/navAndEditButtonGroup.h"
#include "gui/toolbars/mainToolbar/openDualViewButton.hpp"
#include "gui/toolbars/mainToolbar/openSingleViewButton.hpp"
#include "gui/toolbars/mainToolbar/openViewGroupButton.hpp"
#include "gui/toolbars/mainToolbar/refreshVolumeButton.h"
#include "gui/toolbars/mainToolbar/saveButton.h"
#include "gui/toolbars/mainToolbar/showPaintTools.hpp"
#include "gui/toolbars/mainToolbar/showAnnotationsTools.hpp"

MainToolBar::MainToolBar(MainWindow* mainWindow)
    : QToolBar("Tools", mainWindow),
      mainWindow_(mainWindow),
      saveButton(new SaveButton(this)),
      openViewGroupButton_(new OpenViewGroupButton(mainWindow)),
      openSingleViewButton_(new OpenSingleViewButton(mainWindow)),
      openDoubleViewButton_(new OpenDualViewButton(mainWindow)),
      volumeRefreshButton(new VolumeRefreshButton(mainWindow)),
      navEditButtons_(new NavAndEditButtonGroup(mainWindow)) {
  mainWindow_->addToolbarTop(this);

  addWidget(saveButton);
  addSeparator();

  addNavEditButtons();
  addSeparator();

  addWidget(openSingleViewButton_);
  addWidget(openDoubleViewButton_);
  addWidget(openViewGroupButton_);
  // addWidget(volumeRefreshButton);
  addSeparator();

  addWidget(new QLabel("Alpha Level: ", this));
  FilterWidget::Create();
  addWidget(FilterWidget::Widget());

  addWidget(new FilterToBlackCheckbox(this));

  addWidget(new ShowPaintTools(this, mainWindow_->GetViewGroupState()));
  addWidget(new ShowAnnotationsTools(this, mainWindow_->GetViewGroupState()));
}

void MainToolBar::EnableWidgets() {
  const bool toBeEnabled = !OmProject::IsReadOnly();
  saveButton->setEnabled(toBeEnabled);
  navEditButtons_->EnableModifyingWidgets(toBeEnabled);
}

void MainToolBar::UpdateToolbar() {
  EnableWidgets();
}

void MainToolBar::SetTool(const om::tool::mode tool) {
  navEditButtons_->SetTool(tool);
}

void MainToolBar::addNavEditButtons() {
  Q_FOREACH(QAbstractButton * b, navEditButtons_->buttons()) { addWidget(b); }
}
