#include "actions/io/omActionReplayer.hpp"
#include "actions/io/omActionLogger.hpp"
#include "actions/io/omActionOperators.h"

#include "actions/details/omActionImpls.hpp"
#include "actions/details/omSegmentValidateAction.h"
#include "actions/details/omSegmentValidateActionImpl.hpp"
#include "actions/details/omSegmentSplitActionImpl.hpp"
#include "actions/details/omSegmentMultiSplitActionImpl.hpp"
#include "actions/details/omSegmentCutAction.h"
#include "actions/details/omSegmentCutActionImpl.hpp"
#include "actions/details/omSegmentUncertainAction.h"
#include "actions/details/omSegmentUncertainActionImpl.hpp"
#include "actions/details/omVoxelSetValueAction.h"
#include "actions/details/omVoxelSetValueActionImpl.hpp"

OmActionReplayer::OmActionReplayer() {}

void OmActionReplayer::Replay() {
  OmActionReplayer replayer;
  replayer.doReplay();
}

void OmActionReplayer::replayFile(const QFileInfo& fileInfo) {
  QFile file(fileInfo.absoluteFilePath());
  if (!file.open(QIODevice::ReadOnly)) {
    throw om::IoException("could not open");
  }

  QDataStream in(&file);
  in.setByteOrder(QDataStream::LittleEndian);
  in.setVersion(QDataStream::Qt_4_6);

  int logVersion;
  in >> logVersion;
  if (logVersion < 2) {  // in version 1, QString actionName not stored...
    return;
  }

  QString actionName;
  in >> actionName;

  log_info("replaying %s: %i", qPrintable(fileInfo.fileName()), logVersion);

  dispatchAction(actionName, in);

  QString postfix;
  in >> postfix;
  if ("OMNI_LOG" != postfix) {
    throw om::IoException("bad postfix");
  }

  if (!in.atEnd()) {
    throw om::IoException("corrupt log file dectected");
  }
}

void OmActionReplayer::dispatchAction(const QString& actionName,
                                      QDataStream& in) {
  switch (om::actions_::actionStrToType_.at(actionName)) {
    case om::actions_::OmAutomaticSpreadingThresholdChangeAction:
        doReplayFile<OmAutomaticSpreadingThresholdChangeAction, OmAutomaticSpreadingThresholdChangeActionImpl>(in);
        break;
    case om::actions_::OmSegmentValidateAction:
      doReplayFile<OmSegmentValidateAction, OmSegmentValidateActionImpl>(in);
      break;
    case om::actions_::OmSegmentSplitAction:
      doReplayFile<OmSegmentSplitAction, OmSegmentSplitActionImpl>(in);
      break;
    case om::actions_::OmSegmentMultiSplitAction:
      doReplayFile<OmSegmentMultiSplitAction, OmSegmentMultiSplitActionImpl>(in);
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
      throw om::ArgException("should not have received this action");
    default:
      // TODO: or skip it?
      throw om::ArgException("unknown action");
  }; } void OmActionReplayer::doReplay() {
  log_infos << "checking for replay...";
  QDir logdir = OmActionLogger::LogFolder();

  static const QString lockName = ".action.replay.lock";

  // Failsafe incase crashed during last recovery
  if (logdir.exists(lockName)) {
    OmActions::GenerateCloseAction();
    logdir.rmdir(lockName);
    log_infos << "previous replay failed; not replaying";
    return;
  }

  logdir.mkdir(lockName);

  logdir.setFilter(QDir::Files);
  logdir.setSorting(QDir::Name);  // oldest files first

  const std::list<QFileInfo> files = logdir.entryInfoList().toStdList();
  std::list<QFileInfo> filesToReplay;

  FOR_EACH_R(iter, files) {
    if (iter->fileName().contains("OmProjectCloseAction") ||
        iter->fileName().contains("OmProjectSaveAction")) {
      break;
    }
    filesToReplay.push_front(*iter);
  }

  if (0 == filesToReplay.size()) {
    log_infos << "nothing to replay";
  } else {
    FOR_EACH(iter, filesToReplay) { replayFile(*iter); }
    log_infos << "replayed " << filesToReplay.size() << " files";
  }

  logdir.rmdir(lockName);
}
