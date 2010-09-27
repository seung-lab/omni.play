#include "gui/toolbars/dendToolbar/autoBreakCheckbox.h"
#include "gui/toolbars/dendToolbar/graphTools.h"
#include "common/omDebug.h"
#include "system/viewGroup/omViewGroupState.h"

AutoBreakCheckbox::AutoBreakCheckbox( GraphTools * d )
	: OmCheckBox<GraphTools>(d, "Show Breaks")
{
	setCheckState(Qt::Checked);
}

void AutoBreakCheckbox::doAction()
{
	//debug(dendbar, "DendToolBar::autoBreakChecked (%i)\n", isChecked());
	mParent->getViewGroupState()->SetBreakOnSplitMode(isChecked());
}
