#include "actions/io/omActionReplayer.hpp"
#include "actions/io/omActionLogger.hpp"
#include "actions/io/omActionOperators.h"

#include "actions/details/omSegmentValidateAction.h"
#include "actions/details/omSegmentValidateActionImpl.hpp"
#include "actions/details/omSegmentSplitAction.h"
#include "actions/details/omSegmentSplitActionImpl.hpp"
#include "actions/details/omSegmentJoinAction.h"
#include "actions/details/omSegmentJoinActionImpl.hpp"
#include "actions/details/omSegmentSelectAction.h"
#include "actions/details/omSegmentSelectActionImpl.hpp"
#include "actions/details/omSegmentGroupAction.h"
#include "actions/details/omSegmentGroupActionImpl.hpp"
#include "actions/details/omSegmentUncertainAction.h"
#include "actions/details/omSegmentUncertainActionImpl.hpp"
#include "actions/details/omVoxelSetValueAction.h"
#include "actions/details/omVoxelSetValueActionImpl.hpp"
#include "actions/details/omSegmentationThresholdChangeAction.h"
#include "actions/details/omSegmentationThresholdChangeActionImpl.hpp"

OmActionReplayer::OmActionReplayer()
{
	registerActionTypes();
}

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
	in.setByteOrder( QDataStream::LittleEndian );
	in.setVersion(QDataStream::Qt_4_6);

	int logVersion;
	in >> logVersion;
	if(logVersion < 2){ // in version 1, QString actionName not stored...
		return;
	}

	QString actionName;
	in >> actionName;

	printf("replaying %s: %i\n", qPrintable(fileInfo.fileName()),
		   logVersion);

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

// TODO: use BOOST_ENUM_VALUES?
void OmActionReplayer::registerActionTypes()
{
	actionStrToType_["OmSegmentValidateAction"]
		= om::actionReplayer_::OmSegmentValidateAction;
	actionStrToType_["OmSegmentSplitAction"]
		= om::actionReplayer_::OmSegmentSplitAction;
	actionStrToType_["OmSegmentJoinAction"]
		= om::actionReplayer_::OmSegmentJoinAction;
	actionStrToType_["OmSegmentSelectAction"]
		= om::actionReplayer_::OmSegmentSelectAction;
	actionStrToType_["OmSegmentGroupAction"]
		= om::actionReplayer_::OmSegmentGroupAction;
	actionStrToType_["OmSegmentUncertainAction"]
		= om::actionReplayer_::OmSegmentUncertainAction;
	actionStrToType_["OmVolxelSetvalueAction"]
		= om::actionReplayer_::OmVolxelSetvalueAction;
	actionStrToType_["OmSegmentationThresholdChangeAction"] =
		om::actionReplayer_::OmSegmentationThresholdChangeAction;
	actionStrToType_["OmProjectCloseAction"]
		= om::actionReplayer_::OmProjectCloseAction;
	actionStrToType_["OmProjectSaveAction"]
		= om::actionReplayer_::OmProjectSaveAction;
}

void OmActionReplayer::dispatchAction(const QString& actionName,
									  QDataStream& in)
{
	switch(actionStrToType_[actionName]){
	case om::actionReplayer_::OmSegmentValidateAction:
		doReplayFile<OmSegmentValidateAction, OmSegmentValidateActionImpl>(in);
		break;
	case om::actionReplayer_::OmSegmentSplitAction:
		doReplayFile<OmSegmentSplitAction, OmSegmentSplitActionImpl>(in);
		break;
	case om::actionReplayer_::OmSegmentJoinAction:
		doReplayFile<OmSegmentJoinAction, OmSegmentJoinActionImpl>(in);
		break;
	case om::actionReplayer_::OmSegmentSelectAction:
		doReplayFile<OmSegmentSelectAction, OmSegmentSelectActionImpl>(in);
		break;
	case om::actionReplayer_::OmSegmentGroupAction:
		doReplayFile<OmSegmentGroupAction, OmSegmentGroupActionImpl>(in);
		break;
	case om::actionReplayer_::OmSegmentUncertainAction:
		doReplayFile<OmSegmentUncertainAction, OmSegmentUncertainActionImpl>(in);
		break;
	case om::actionReplayer_::OmVolxelSetvalueAction:
		doReplayFile<OmVoxelSetValueAction, OmVoxelSetValueActionImpl>(in);
		break;
	case om::actionReplayer_::OmSegmentationThresholdChangeAction:
		doReplayFile<OmSegmentationThresholdChangeAction,
			OmSegmentationThresholdChangeActionImpl>(in);
		break;
	case om::actionReplayer_::OmProjectCloseAction:
	case om::actionReplayer_::OmProjectSaveAction:
		throw OmArgException("should not have received this action");
	default:
		throw OmArgException("unknown action", actionName);
	};
}

void OmActionReplayer::doReplay()
{
	printf("checking for replay...\n");
	QDir& logdir = OmActionLogger::LogFolder();

	static const QString lockName = ".action.replay.lock";

	// Failsafe incase crashed during last recovery
	if(logdir.exists(lockName)) {
		OmActions::Close();
		logdir.rmdir(lockName);
		printf("previous replay failed; not replaying\n");
		return;
	}

	logdir.mkdir(lockName);

	logdir.setFilter(QDir::Files);
	logdir.setSorting(QDir::Name); // oldest files first

	const std::list<QFileInfo> files = logdir.entryInfoList().toStdList();
	std::list<QFileInfo> filesToReplay;

	FOR_EACH_R(iter, files){
		if(iter->fileName().contains("OmProjectCloseAction") ||
		   iter->fileName().contains("OmProjectSaveAction")) {
			break;
		}
		filesToReplay.push_front(*iter);
	}

	if(0 == filesToReplay.size()){
		printf("nothing to replay\n");
	} else {
		FOR_EACH(iter, filesToReplay){
			replayFile(*iter);
		}
		std::cout << "replayed " << filesToReplay.size() << " files\n";
	}

	logdir.rmdir(lockName);
}
