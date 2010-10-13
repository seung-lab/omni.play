#ifndef OM_ACTION_H
#define OM_ACTION_H

#include <QUndoCommand>
#include <string>

class OmAction : public QUndoCommand {

public:
	void Run();

protected:
	OmAction();
	void SetValid(bool);
	void SetUndoable(bool);
	void SetActivate(bool);
	bool GetActivate();

	virtual void Action() = 0;
	virtual void UndoAction() = 0;
	virtual std::string Description() = 0;
	virtual void save(const std::string&) = 0;
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
