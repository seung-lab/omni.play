#include "gui/toolbars/mainToolbar/toolButton.h"
#include "system/omEvents.h"
#include "system/omStateManager.h"

ToolButton::ToolButton(QWidget * parent,
		       const QString & title, 
		       const QString & statusTip,
		       const OmToolMode tool)
	: OmButton<QWidget>( parent, 
			     title,
			     statusTip,
			     true)
	, mTool(tool)
{
	setFlat(true);
}

void ToolButton::doAction()
{
	OmStateManager::SetToolModeAndSendEvent(mTool);
}

