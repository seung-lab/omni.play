
#include "omAction.h"

#include "system/omEventManager.h"
#include "system/events/omSegmentEvent.h"
#include "system/omStateManager.h"
#include "common/omDebug.h"

#define DEBUG 0

#pragma mark -
#pragma mark Example Class
/////////////////////////////////
///////
///////          Example Class
///////

OmAction::OmAction()
{
	//assume valid
	mValid = true;
	//assume undoable
	mUndoable = true;
}

void
 OmAction::SetValid(bool state)
{
	mValid = state;
}

void OmAction::SetUndoable(bool state)
{
	mUndoable = state;
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
		OmStateManager::ClearUndoStack();
	}
}

#pragma mark
#pragma mark Example Methods
/////////////////////////////////
///////          Example Methods

void OmAction::redo()
{
	Action();
}

void OmAction::undo()
{
	UndoAction();
}
