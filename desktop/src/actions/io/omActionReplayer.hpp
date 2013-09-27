#pragma once

#include "common/common.h"
#include "actions/io/omActionTypes.h"

#include <QFileInfo>

class OmActionReplayer {
public:
    static void Replay();

private:
    OmActionReplayer();

    void replayFile(const QFileInfo& file);
    void doReplay();

    void dispatchAction(const QString& actionName, QDataStream& in);

    template <typename ACTION, typename IMPL>
    void doReplayFile(QDataStream& in)
    {
        std::shared_ptr<IMPL> impl(new IMPL());
        in >> (*impl);

        // action will be deleted by QUndoState...
        ACTION* action = new ACTION(impl);
        action->Replay();
    }
};

