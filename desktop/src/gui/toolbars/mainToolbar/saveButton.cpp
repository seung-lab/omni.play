#include "gui/toolbars/mainToolbar/saveButton.h"
#include "project/omProject.h"
#include "actions/omActions.h"

SaveButton::SaveButton(MainToolBar * parent)
	: OmButton<MainToolBar>( parent,
				 "Save",
				 "Saves the current project",
				 false)
{
	setFlat(true);
	setKeyboardShortcut("Ctrl+S");
	setIconAndText(":/toolbars/mainToolbar/icons/1277961998_3floppy_unmount.png");
}

void SaveButton::doAction()
{
	OmActions::Save();
}
