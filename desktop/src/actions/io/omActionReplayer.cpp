#include "actions/io/omActionReplayer.hpp"
#include "actions/io/omActionLogger.hpp"
#include "actions/io/omActionOperators.h"

#include "actions/details/omActionImpls.hpp"
#include "actions/details/omSegmentValidateAction.h"
#include "actions/details/omSegmentValidateActionImpl.hpp"
#include "actions/details/omSegmentSplitAction.h"
#include "actions/details/omSegmentSplitActionImpl.hpp"
#include "actions/details/omSegmentCutAction.h"
#include "actions/details/omSegmentCutActionImpl.hpp"
#include "actions/details/omSegmentUncertainAction.h"
#include "actions/details/omSegmentUncertainActionImpl.hpp"
#include "actions/details/omVoxelSetValueAction.h"
#include "actions/details/omVoxelSetValueActionImpl.hpp"

OmActionReplayer::OmActionReplayer()
{}

void OmActionReplayer::Replay()
{
    OmActionReplayer replayer;
    replayer.doReplay();
}

void OmActionReplayer::replayFile(const QFileInfo& fileInfo)
{
    QFile file(fileInfo.absoluteFilePath());
    if(!file.open(QIODevice::ReadOnly)){
        throw OmIoException("could not open", file.fileName());
    }

    QDataStream in(&file);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setVersion(QDataStream::Qt_4_6);

    int logVersion;
    in >> logVersion;
    if(logVersion < 2){ // in version 1, QString actionName not stored...
        return;
    }

    QString actionName;
    in >> actionName;

    printf("replaying %s: %i\n", qPrintable(fileInfo.fileName()), logVersion);

    dispatchAction(actionName, in);

    QString postfix;
    in >> postfix;
    if("OMNI_LOG" != postfix){
        throw OmIoException("bad postfix", file.fileName());
    }

    if(!in.atEnd()){
        throw OmIoException("corrupt log file dectected", file.fileName());
    }
}

void OmActionReplayer::dispatchAction(const QString& actionName,
                                      QDataStream& in)
{
    switch(om::actions_::actionStrToType_.at(actionName)){
    case om::actions_::OmSegmentValidateAction:
        doReplayFile<OmSegmentValidateAction, OmSegmentValidateActionImpl>(in);
        break;
    case om::actions_::OmSegmentSplitAction:
        doReplayFile<OmSegmentSplitAction, OmSegmentSplitActionImpl>(in);
        break;
    case om::actions_::OmSegmentCutAction:
        doReplayFile<OmSegmentCutAction, OmSegmentCutActionImpl>(in);
        break;
    case om::actions_::OmSegmentJoinAction:
        doReplayFile<OmSegmentJoinAction, OmSegmentJoinActionImpl>(in);
        break;
    case om::actions_::OmSegmentSelectAction:
        doReplayFile<OmSegmentSelectAction, OmSegmentSelectActionImpl>(in);
        break;
    case om::actions_::OmSegmentGroupAction:
        doReplayFile<OmSegmentGroupAction, OmSegmentGroupActionImpl>(in);
        break;
    case om::actions_::OmSegmentUncertainAction:
        doReplayFile<OmSegmentUncertainAction, OmSegmentUncertainActionImpl>(in);
        break;
    case om::actions_::OmVolxelSetvalueAction:
        doReplayFile<OmVoxelSetValueAction, OmVoxelSetValueActionImpl>(in);
        break;
    case om::actions_::OmSegmentationThresholdChangeAction:
        doReplayFile<OmSegmentationThresholdChangeAction,
            OmSegmentationThresholdChangeActionImpl>(in);
        break;
    case om::actions_::OmProjectCloseAction:
    case om::actions_::OmProjectSaveAction:
        throw OmArgException("should not have received this action");
    default:
        throw OmArgException("unknown action", actionName);
    };
}

void OmActionReplayer::doReplay()
{
    printf("checking for replay...\n");
    QDir logdir = OmActionLogger::LogFolder();

    static const QString lockName = ".action.replay.lock";

    // Failsafe incase crashed during last recovery
    if(logdir.exists(lockName))
    {
        OmActions::GenerateCloseAction();
        logdir.rmdir(lockName);
        printf("previous replay failed; not replaying\n");
        return;
    }

    logdir.mkdir(lockName);

    logdir.setFilter(QDir::Files);
    logdir.setSorting(QDir::Name); // oldest files first

    const std::list<QFileInfo> files = logdir.entryInfoList().toStdList();
    std::list<QFileInfo> filesToReplay;

    FOR_EACH_R(iter, files)
    {
        if(iter->fileName().contains("OmProjectCloseAction") ||
           iter->fileName().contains("OmProjectSaveAction")) {
            break;
        }
        filesToReplay.push_front(*iter);
    }

    if(0 == filesToReplay.size()){
        printf("nothing to replay\n");
    } else
    {
        FOR_EACH(iter, filesToReplay){
            replayFile(*iter);
        }
        std::cout << "replayed " << filesToReplay.size() << " files\n";
    }

    logdir.rmdir(lockName);
}
