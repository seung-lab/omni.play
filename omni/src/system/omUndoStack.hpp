#ifndef OM_UNDO_STACK_HPP
#define OM_UNDO_STACK_HPP

#include "common/omCommon.h"
#include "system/omConnect.hpp"

#include <QUndoStack>
#include <QShortcut>

class OmUndoStack : private QObject{
Q_OBJECT

private:
    QUndoStack undoStack_;
    boost::scoped_ptr<QShortcut> undoShortcut_;

private Q_SLOTS:
    void push(QUndoCommand* cmd){
        undoStack_.push(cmd);
    }

    void clear(){
        undoStack_.clear();
    }

    void undo()
    {
        std::cout << "undoing action...\n";
        undoStack_.undo();
        std::cout << "done undoing action\n";
    }

Q_SIGNALS:
    void signalPush(QUndoCommand* cmd);
    void signalClear();
    void signalUndo();

public:
    OmUndoStack()
    {
        om::connect(this, SIGNAL(signalPush(QUndoCommand*)),
                    this, SLOT(push(QUndoCommand*)));

        om::connect(this, SIGNAL(signalClear()),
                    this, SLOT(clear()));

        om::connect(this, SIGNAL(signalUndo()),
                    this, SLOT(undo()));
    }

   void SetGlobalShortcut(QWidget* parent)
    {
        undoShortcut_.reset(new QShortcut(parent));
        undoShortcut_->setKey(Qt::CTRL + Qt::Key_Z);
        undoShortcut_->setContext(Qt::ApplicationShortcut);

        om::connect(undoShortcut_.get(), SIGNAL(activated()),
                    this, SLOT(undo()));
    }

    ~OmUndoStack()
    {}

    inline QUndoStack* Get() {
        return &undoStack_;
    }

    inline void Push(QUndoCommand* cmd){
        signalPush(cmd);
    }

    inline void Clear(){
        signalClear();
    }

    inline void Undo(){
        signalUndo();
    }
};

#endif
