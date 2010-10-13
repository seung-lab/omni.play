#include "omAction.h"
#include "system/omStateManager.h"

OmAction::OmAction()
	: mUndoable(true)
	, mValid(true)
	, mActivate(true)
{
}

void OmAction::SetValid(bool state)
{
	mValid = state;
}

void OmAction::SetUndoable(bool state)
{
	mUndoable = state;
}

void OmAction::SetActivate(bool state)
{
	mActivate = state;
}

bool OmAction::GetActivate()
{
	return mActivate;
}

/*
 *	Performs action if it is valid.  This method always
 *	gives up responsibility of the object pointer.
 */
void OmAction::Run()
{
	//if invalid action
	if (!mValid) {
		//delete self
		delete this;
		return;
	}

	//set description
	setText(QString::fromStdString(Description()));

	//push string on undo stack
	OmStateManager::PushUndoCommand(this);

	//if not undoable
	if (!mUndoable) {
		OmStateManager::UndoUndoCommand();
	}
}

void OmAction::redo()
{
	setText(QString::fromStdString(Description()));

	// Only do the action if the action is activated.
	if(GetActivate()) {
		Action();
		save("executeOrRedo");
	} else {
		SetActivate(true);
	}
}

void OmAction::undo()
{
	UndoAction();
	setText(QString::fromStdString(Description()));
	save("undo");
}
