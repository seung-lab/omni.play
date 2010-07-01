#include "gui/toolbars/mainToolbar/saveButton.h"
#include "project/omProject.h"

SaveButton::SaveButton(MainToolbar * parent)
	: OmButton<MainToolbar>( parent, 
				 "Save", 
				 "Saves the current project", 
				 false)
{
	setFlat(true);
	setKeyboardShortcut("Ctrl+S");
	setIcon(QIcon(":/toolbars/mainToolbar/icons/1277961998_3floppy_unmount.png"));
	setText("");
}

void SaveButton::doAction()
{
	OmProject::Save();
}
