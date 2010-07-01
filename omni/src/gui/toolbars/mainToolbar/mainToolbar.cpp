#include "gui/mainwindow.h"
#include "gui/toolbars/mainToolbar/filterWidget.h"
#include "gui/toolbars/mainToolbar/mainToolbar.h"
#include "gui/toolbars/mainToolbar/navButtons.h"
#include "gui/toolbars/mainToolbar/openViewGroupButton.h"
#include "gui/toolbars/mainToolbar/refreshVolumeButton.h"
#include "gui/toolbars/mainToolbar/saveButton.h"
#include "system/omProjectData.h"

MainToolbar::MainToolbar( MainWindow * mw )
	: QWidget(mw)
	, mMainWindow(mw)
	, saveButton(new SaveButton(this))
	, buttons(new NavAndEditButtons(mw))
	, openViewGroupButton(new OpenViewGroupButton(mw))
	, filterWidget(new FilterWidget(mw))
	, volumeRefreshButton(new VolumeRefreshButton(mw))
{
	addWidgetToToolbar(saveButton);
	addSeperatorToToolbar();
	buttons->addTheButtons();
	addSeperatorToToolbar();
	addWidgetToToolbar(openViewGroupButton);
	addWidgetToToolbar(volumeRefreshButton);
	addSeperatorToToolbar();
	addWidgetToToolbar(new QLabel("Alpha Level: ",this));
	addWidgetToToolbar(filterWidget);
}

void MainToolbar::addWidgetToToolbar( QWidget * widget )
{
	mMainWindow->addToolbarWidget(widget);
}

void MainToolbar::addSeperatorToToolbar()
{
	mMainWindow->addToolbarSeperator();
}

void MainToolbar::setReadOnlyWidgetsEnabled()
{
	bool toBeEnabled = false;
	if( mMainWindow->isProjectOpen() ){
		toBeEnabled = true;
	}

	saveButton->setEnabled(toBeEnabled);
	buttons->setReadOnlyWidgetsEnabled(toBeEnabled);
	openViewGroupButton->setEnabled(toBeEnabled);
	filterWidget->setEnabled(toBeEnabled);
}

void MainToolbar::setModifyWidgetsEnabled()
{
	bool toBeEnabled = false;
	if( mMainWindow->isProjectOpen() && !OmProjectData::IsReadOnly() ){
		toBeEnabled = true;
	}

	buttons->setModifyWidgetsEnabled(toBeEnabled);
}

void MainToolbar::updateToolbar()
{
	setReadOnlyWidgetsEnabled();
	setModifyWidgetsEnabled();
}
