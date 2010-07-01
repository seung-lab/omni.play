#include "gui/toolbars/mainToolbar/toolButton.h"
#include "system/omEvents.h"
#include "system/omStateManager.h"

ToolButton::ToolButton(QWidget * parent,
		       const QString & title, 
		       const QString & statusTip,
		       const OmToolMode tool,
		       const QString & iconPath)
	: OmButton<QWidget>( parent, 
			     title,
			     statusTip,
			     true)
	, mTool(tool)
{
	setFlat(true);
	setIconAndText(iconPath);
}

void ToolButton::doAction()
{
	OmStateManager::SetToolModeAndSendEvent(mTool);
}

