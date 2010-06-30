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
}

void SaveButton::doAction()
{
	OmProject::Save();
}
