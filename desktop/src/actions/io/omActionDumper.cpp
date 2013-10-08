#include "actions/io/omActionDumper.h"
#include "actions/io/omActionLogger.hpp"

#include "actions/io/omActionOperatorsText.h"

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

void OmActionDumper::Dump(const QString& fnp) {
  QFile file(fnp);
  if (!file.open(QIODevice::WriteOnly)) {
    throw OmIoException("could not open", fnp);
  }

  out_.reset(new QTextStream(&file));

  QDir logdir = OmActionLogger::LogFolder();
  logdir.setFilter(QDir::Files);
  logdir.setSorting(QDir::Name);  // oldest files first

  const std::list<QFileInfo> files = logdir.entryInfoList().toStdList();

  FOR_EACH(iter, files) { readAndDumpFile(*iter); }
}

void OmActionDumper::readAndDumpFile(const QFileInfo& fileInfo) {
  QFile file(fileInfo.absoluteFilePath());
  if (!file.open(QIODevice::ReadOnly)) {
    throw OmIoException("could not open", file.fileName());
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

  printf("dumping %s: %i\n", qPrintable(fileInfo.fileName()), logVersion);

  dispatchAction(actionName, in, fileInfo.fileName());

  QString postfix;
  in >> postfix;
  if ("OMNI_LOG" != postfix) {
    throw OmIoException("bad postfix", file.fileName());
  }

  if (!in.atEnd()) {
    throw OmIoException("corrupt log file dectected", file.fileName());
  }
}

void OmActionDumper::dispatchAction(const QString& actionName, QDataStream& in,
                                    const QString& fnp) {
  switch (om::actions_::actionStrToType_.at(actionName)) {
    case om::actions_::OmSegmentValidateAction:
      doDumpFile<OmSegmentValidateAction, OmSegmentValidateActionImpl>(in, fnp);
      break;
    case om::actions_::OmSegmentSplitAction:
      doDumpFile<OmSegmentSplitAction, OmSegmentSplitActionImpl>(in, fnp);
      break;
    case om::actions_::OmSegmentCutAction:
      doDumpFile<OmSegmentCutAction, OmSegmentCutActionImpl>(in, fnp);
      break;
    case om::actions_::OmSegmentJoinAction:
      doDumpFile<OmSegmentJoinAction, OmSegmentJoinActionImpl>(in, fnp);
      break;
    case om::actions_::OmSegmentSelectAction:
      doDumpFile<OmSegmentSelectAction, OmSegmentSelectActionImpl>(in, fnp);
      break;
    case om::actions_::OmSegmentGroupAction:
      doDumpFile<OmSegmentGroupAction, OmSegmentGroupActionImpl>(in, fnp);
      break;
    case om::actions_::OmSegmentUncertainAction:
      doDumpFile<OmSegmentUncertainAction, OmSegmentUncertainActionImpl>(in,
                                                                         fnp);
      break;
    case om::actions_::OmVolxelSetvalueAction:
      doDumpFile<OmVoxelSetValueAction, OmVoxelSetValueActionImpl>(in, fnp);
      break;
    case om::actions_::OmSegmentationThresholdChangeAction:
      doDumpFile<OmSegmentationThresholdChangeAction,
                 OmSegmentationThresholdChangeActionImpl>(in, fnp);
      break;
    case om::actions_::OmProjectCloseAction:
      doDumpFile<OmProjectCloseAction, OmProjectCloseActionImpl>(in, fnp);
      break;
    case om::actions_::OmProjectSaveAction:
      doDumpFile<OmProjectSaveAction, OmProjectSaveActionImpl>(in, fnp);
      break;
    default:
      throw OmArgException("unknown action", actionName);
  }
  ;
}
