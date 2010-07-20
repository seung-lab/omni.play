#include "gui/mainwindow.h"
#include "gui/toolbars/mainToolbar/navAndEditButtonGroup.h"
#include "gui/toolbars/mainToolbar/navAndEditButtons.h"
#include "gui/toolbars/mainToolbar/toolButton.h"

NavAndEditButtons::NavAndEditButtons(MainWindow * mw)
	: QWidget(mw)
	, mMainWindow(mw)
	, mButtonGroup(new NavAndEditButtonGroup(mw))
{
}

NavAndEditButtons::~NavAndEditButtons()
{
	delete mButtonGroup;
}

void NavAndEditButtons::addTheButtons()
{
	foreach( QAbstractButton * b, mButtonGroup->buttons() ){
		mMainWindow->addToolbarWidget(b);		
	}
}

void NavAndEditButtons::setReadOnlyWidgetsEnabled(const bool toBeEnabled)
{
	mButtonGroup->setReadOnlyWidgetsEnabled(toBeEnabled);
}

void NavAndEditButtons::setModifyWidgetsEnabled(const bool toBeEnabled)
{
	mButtonGroup->setModifyWidgetsEnabled(toBeEnabled);
}

void NavAndEditButtons::setTool(const OmToolMode tool)
{
	mButtonGroup->setTool(tool);
}
