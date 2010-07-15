#ifndef OM_ACTION_H
#define OM_ACTION_H

#include "common/omCommon.h"
#include <QUndoCommand>

class OmAction : public QUndoCommand {

public:
	void Run();
	
protected:
	OmAction();
	void SetValid(bool);
	void SetUndoable(bool);
	void SetActivate(bool);
	bool GetActivate();
	
	//action interface
	virtual void Action() = 0;
	virtual void UndoAction() = 0;
	virtual string Description() = 0;
	virtual void save(const string & comment ) = 0;
	virtual QString classNameForLogFile() = 0;

	bool mUndoable;

private:
	//wrapped methods
	void redo();
	void undo();
	
	//action properties
	bool mValid;
	bool mActivate;
};

#endif
