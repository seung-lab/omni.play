#ifndef OM_ACTION_H
#define OM_ACTION_H

/*
 *
 *
 */

#include "common/omStd.h"

#include <QUndoCommand>

class OmAction:public QUndoCommand {

 public:
	void Run();

 protected:
	 OmAction();
	void SetValid(bool);
	void SetUndoable(bool);

	//action interface
	virtual void Action() = 0;
	virtual void UndoAction() = 0;
	virtual string Description() = 0;

	bool mUndoable;
 private:
	//wrapped methods
	void redo();
	void undo();

	//action properties
	bool mValid;
};

#endif
