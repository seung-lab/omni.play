#ifndef OM_ACTION_H
#define OM_ACTION_H

#include "system/omStateManager.h"

#include <QUndoCommand>

class OmUndoCommand : public QUndoCommand {
public:
    virtual ~OmUndoCommand()
    {}

    /*
     * Performs action if it is valid.  This method always
     *   gives up responsibility of the object pointer.
     */
    void Run()
    {
        //if invalid action
        if (!mValid) {
            //delete self
            delete this;
            return;
        }

        SetDescription();

        //push string on undo stack
        OmStateManager::PushUndoCommand(this);

        //if not undoable
        if(!mUndoable) {
            OmStateManager::UndoUndoCommand();
        }
    }

    void Replay()
    {
        SetActivate(false);
        OmStateManager::PushUndoCommand(this);
        Action();
    }

protected:
    bool mUndoable;

    OmUndoCommand()
        : mUndoable(true)
        , mValid(true)
        , mActivate(true)
    {}

    void SetDescription(){
        setText(QString::fromStdString(Description()));
    }

    void SetDescription(const QString& str){
        setText(str);
    }

    void SetValid(bool state){
        mValid = state;
    }

    void SetUndoable(bool state){
        mUndoable = state;
    }

    void SetActivate(bool state){
        mActivate = state;
    }

    bool GetActivate() const {
        return mActivate;
    }

    virtual void Action() = 0;
    virtual void UndoAction() = 0;
    virtual std::string Description() = 0;
    virtual void save(const std::string&) = 0;

private:
    //action properties
    bool mValid;
    bool mActivate;

    //wrapped methods
    void redo()
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

    void undo()
    {
        UndoAction();
        setText(QString::fromStdString(Description()));
        save("undo");
    }
};

#endif
