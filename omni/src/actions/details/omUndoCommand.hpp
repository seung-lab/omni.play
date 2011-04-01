#ifndef OM_UNDO_COMMAND_HPP
#define OM_UNDO_COMMAND_HPP

#include "system/omStateManager.h"
#include "system/omUndoStack.hpp"

#include <QUndoCommand>

class OmUndoCommand : public QUndoCommand {
public:
    virtual ~OmUndoCommand()
    {}

    // Performs action if it is valid.  This method always
    //   gives up responsibility of the object pointer.
    void Run()
    {
        if (!mValid) {
            delete this;
            return;
        }

        SetDescription();

        if(mUndoable) {
            //push string on undo stack (causing redo to be called)
            OmStateManager::UndoStack().Push(this);
        } else {
            // call redo outselves
            redo();
        }
    }

    // for Project Close and Save (which should not go through undo stack)
    void RunNow()
    {
        SetDescription();
        Action();
        save("executeOrRedo");
    }

    void Replay()
    {
        SetActivate(false);
        OmStateManager::UndoStack().Push(this);
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

    void SetValid(const bool state){
        mValid = state;
    }

    void SetUndoable(const bool state){
        mUndoable = state;
    }

    void SetActivate(const bool state){
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
    bool mValid;
    bool mActivate;

    void redo()
    {
        setText(QString::fromStdString(Description()));

        // Only do the action if the action is activated.
        if(mActivate) {
            Action();
            save("executeOrRedo");
        } else {
            mActivate = true;
        }
    }

    void undo()
    {
        if(mUndoable)
        {
            UndoAction();
            setText(QString::fromStdString(Description()));
            save("undo");
        }
    }
};

#endif
